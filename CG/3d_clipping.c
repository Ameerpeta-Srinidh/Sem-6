#include <stdio.h>
#include <stdlib.h>
#include <process.h>

#define GNUPLOT_PATH "\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" -persistent"

typedef struct {
    float x;
    float y;
    float z;
} Point3D;

enum {
    IN3D = 0,
    LEFT3D = 1,
    RIGHT3D = 2,
    BOTTOM3D = 4,
    TOP3D = 8,
    NEAR3D = 16,
    FAR3D = 32
};

static int compute_code_3d(
    float x,
    float y,
    float z,
    float xmin,
    float ymin,
    float zmin,
    float xmax,
    float ymax,
    float zmax
) {
    int code = IN3D;

    if (x < xmin) {
        code |= LEFT3D;
    } else if (x > xmax) {
        code |= RIGHT3D;
    }

    if (y < ymin) {
        code |= BOTTOM3D;
    } else if (y > ymax) {
        code |= TOP3D;
    }

    if (z < zmin) {
        code |= NEAR3D;
    } else if (z > zmax) {
        code |= FAR3D;
    }

    return code;
}

static int cohen_sutherland_clip_3d(
    Point3D *p1,
    Point3D *p2,
    float xmin,
    float ymin,
    float zmin,
    float xmax,
    float ymax,
    float zmax
) {
    int code1 = compute_code_3d(p1->x, p1->y, p1->z, xmin, ymin, zmin, xmax, ymax, zmax);
    int code2 = compute_code_3d(p2->x, p2->y, p2->z, xmin, ymin, zmin, xmax, ymax, zmax);

    while (1) {
        if (!(code1 | code2)) {
            return 1;
        }

        if (code1 & code2) {
            return 0;
        }

        int code_out = code1 ? code1 : code2;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        if (code_out & LEFT3D) {
            x = xmin;
            y = p1->y + (p2->y - p1->y) * (xmin - p1->x) / (p2->x - p1->x);
            z = p1->z + (p2->z - p1->z) * (xmin - p1->x) / (p2->x - p1->x);
        } else if (code_out & RIGHT3D) {
            x = xmax;
            y = p1->y + (p2->y - p1->y) * (xmax - p1->x) / (p2->x - p1->x);
            z = p1->z + (p2->z - p1->z) * (xmax - p1->x) / (p2->x - p1->x);
        } else if (code_out & BOTTOM3D) {
            y = ymin;
            x = p1->x + (p2->x - p1->x) * (ymin - p1->y) / (p2->y - p1->y);
            z = p1->z + (p2->z - p1->z) * (ymin - p1->y) / (p2->y - p1->y);
        } else if (code_out & TOP3D) {
            y = ymax;
            x = p1->x + (p2->x - p1->x) * (ymax - p1->y) / (p2->y - p1->y);
            z = p1->z + (p2->z - p1->z) * (ymax - p1->y) / (p2->y - p1->y);
        } else if (code_out & NEAR3D) {
            z = zmin;
            x = p1->x + (p2->x - p1->x) * (zmin - p1->z) / (p2->z - p1->z);
            y = p1->y + (p2->y - p1->y) * (zmin - p1->z) / (p2->z - p1->z);
        } else if (code_out & FAR3D) {
            z = zmax;
            x = p1->x + (p2->x - p1->x) * (zmax - p1->z) / (p2->z - p1->z);
            y = p1->y + (p2->y - p1->y) * (zmax - p1->z) / (p2->z - p1->z);
        }

        if (code_out == code1) {
            p1->x = x;
            p1->y = y;
            p1->z = z;
            code1 = compute_code_3d(p1->x, p1->y, p1->z, xmin, ymin, zmin, xmax, ymax, zmax);
        } else {
            p2->x = x;
            p2->y = y;
            p2->z = z;
            code2 = compute_code_3d(p2->x, p2->y, p2->z, xmin, ymin, zmin, xmax, ymax, zmax);
        }
    }
}

static void write_box_edges(FILE *gp, float xmin, float ymin, float zmin, float xmax, float ymax, float zmax) {
    const Point3D vertices[8] = {
        {xmin, ymin, zmin}, {xmax, ymin, zmin}, {xmax, ymax, zmin}, {xmin, ymax, zmin},
        {xmin, ymin, zmax}, {xmax, ymin, zmax}, {xmax, ymax, zmax}, {xmin, ymax, zmax}
    };
    const int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    for (int i = 0; i < 12; i++) {
        const Point3D a = vertices[edges[i][0]];
        const Point3D b = vertices[edges[i][1]];
        fprintf(gp, "%f %f %f\n", a.x, a.y, a.z);
        fprintf(gp, "%f %f %f\n\n", b.x, b.y, b.z);
    }
}

int main(void) {
    float xmin, ymin, zmin, xmax, ymax, zmax;
    Point3D original_p1, original_p2;
    Point3D clipped_p1, clipped_p2;

    printf("--- 3D Line Clipping using Cohen-Sutherland Algorithm ---\n");
    printf("Enter clipping box (xmin ymin zmin xmax ymax zmax): ");
    if (scanf("%f %f %f %f %f %f", &xmin, &ymin, &zmin, &xmax, &ymax, &zmax) != 6) {
        return 1;
    }

    if (xmin >= xmax || ymin >= ymax || zmin >= zmax) {
        printf("Invalid clipping box.\n");
        return 1;
    }

    printf("Enter line start point (x1 y1 z1): ");
    if (scanf("%f %f %f", &original_p1.x, &original_p1.y, &original_p1.z) != 3) {
        return 1;
    }

    printf("Enter line end point (x2 y2 z2): ");
    if (scanf("%f %f %f", &original_p2.x, &original_p2.y, &original_p2.z) != 3) {
        return 1;
    }

    clipped_p1 = original_p1;
    clipped_p2 = original_p2;

    int accepted = cohen_sutherland_clip_3d(
        &clipped_p1, &clipped_p2, xmin, ymin, zmin, xmax, ymax, zmax
    );

    if (accepted) {
        printf("Clipped line: (%.2f, %.2f, %.2f) to (%.2f, %.2f, %.2f)\n",
               clipped_p1.x, clipped_p1.y, clipped_p1.z,
               clipped_p2.x, clipped_p2.y, clipped_p2.z);
    } else {
        printf("Line rejected: it lies completely outside the clipping box.\n");
    }

    FILE *gp = _popen(GNUPLOT_PATH, "w");
    if (!gp) {
        printf("Error opening gnuplot.\n");
        return 1;
    }

    fprintf(gp, "set terminal windows\n");
    fprintf(gp, "set title '3D Cohen-Sutherland Line Clipping'\n");
    fprintf(gp, "set xlabel 'X'\n");
    fprintf(gp, "set ylabel 'Y'\n");
    fprintf(gp, "set zlabel 'Z'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set view 60, 30, 1, 1\n");
    fprintf(gp, "splot '-' with lines lw 1 lc rgb 'gray50' title 'Original Line', \\\n");
    fprintf(gp, "      '-' with lines lw 3 lc rgb 'red' title 'Clipped Line', \\\n");
    fprintf(gp, "      '-' with lines lw 1 lc rgb 'blue' title 'Clipping Box'\n");

    fprintf(gp, "%f %f %f\n", original_p1.x, original_p1.y, original_p1.z);
    fprintf(gp, "%f %f %f\n", original_p2.x, original_p2.y, original_p2.z);
    fprintf(gp, "e\n");

    if (accepted) {
        fprintf(gp, "%f %f %f\n", clipped_p1.x, clipped_p1.y, clipped_p1.z);
        fprintf(gp, "%f %f %f\n", clipped_p2.x, clipped_p2.y, clipped_p2.z);
    }
    fprintf(gp, "e\n");

    write_box_edges(gp, xmin, ymin, zmin, xmax, ymax, zmax);
    fprintf(gp, "e\n");

    fflush(gp);
    _pclose(gp);
    return 0;
}
