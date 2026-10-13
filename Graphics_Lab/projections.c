#include <stdio.h>
#include <math.h>
#include <string.h>

#define N 8    // Cube has 8 vertices
#define DIM 4  // Homogeneous coordinates (x, y, z, 1)

void drawShape3D(FILE *gp, float shape[N][DIM]) {
    // 12 edges of a cube
    int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0}, // bottom face
        {4,5},{5,6},{6,7},{7,4}, // top face
        {0,4},{1,5},{2,6},{3,7}  // vertical edges
    };
    for (int e = 0; e < 12; e++) {
        int i = edges[e][0], j = edges[e][1];
        fprintf(gp, "%f %f %f\n", shape[i][0], shape[i][1], shape[i][2]);
        fprintf(gp, "%f %f %f\n\n", shape[j][0], shape[j][1], shape[j][2]);
    }
}

void drawShape2D(FILE *gp, float shape[N][DIM]) {
    // Draw cube edges projected onto 2D (x, y only)
    int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };
    for (int e = 0; e < 12; e++) {
        int i = edges[e][0], j = edges[e][1];
        fprintf(gp, "%f %f\n", shape[i][0], shape[i][1]);
        fprintf(gp, "%f %f\n\n", shape[j][0], shape[j][1]);
    }
}

void multiply(float a[N][DIM], float t[DIM][DIM], float result[N][DIM]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < DIM; j++) {
            result[i][j] = 0;
            for (int k = 0; k < DIM; k++)
                result[i][j] += a[i][k] * t[k][j];
        }
}

// Perspective divide: divide x,y by w to get 2D projected points
void perspectiveDivide(float shape[N][DIM], float out[N][DIM]) {
    for (int i = 0; i < N; i++) {
        float w = shape[i][3];
        if (fabs(w) < 1e-6) w = 1e-6; // avoid division by zero
        out[i][0] = shape[i][0] / w;
        out[i][1] = shape[i][1] / w;
        out[i][2] = shape[i][2] / w;
        out[i][3] = 1;
    }
}

int main() {
    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) {
        fprintf(stderr, "Error: Could not open gnuplot.\n");
        return 1;
    }

    // Cube vertices (x, y, z, 1)
    float shape[N][DIM] = {
        {10, 10, 10, 1},
        {30, 10, 10, 1},
        {30, 30, 10, 1},
        {10, 30, 10, 1},
        {10, 10, 30, 1},
        {30, 10, 30, 1},
        {30, 30, 30, 1},
        {10, 30, 30, 1}
    };

    float result[N][DIM];
    float projected[N][DIM];
    int choice;

    printf("\n===== 3D Projections =====\n");
    printf("  1. Orthographic Projection (onto XY plane)\n");
    printf("  2. Oblique Projection (Cavalier/Cabinet)\n");
    printf("  3. Perspective Projection (1-point)\n");
    printf("  4. Isometric Projection\n");
    printf("\nEnter choice (1-4): ");

    if (scanf("%d", &choice) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    switch (choice) {

        // ─────────────────────────────────────────
        // PROJECTIONS
        // ─────────────────────────────────────────

        case 1: {
            // Orthographic: drop Z (project onto XY plane)
            printf("\n[Orthographic Projection onto XY plane]\n");
            float temp[DIM][DIM] = {
                {1, 0, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 0}, // Z zeroed out
                {0, 0, 0, 1}
            };

            multiply(shape, temp, result);

            fprintf(gp, "set xrange [-50:100]\nset yrange [-50:100]\n");
            fprintf(gp, "set xlabel 'X'\nset ylabel 'Y'\nset title 'Orthographic Projection'\n");
            fprintf(gp, "plot '-' with lines title 'Original (XY shadow)', "
                        "'-' with lines title 'Projected'\n");
            drawShape2D(gp, shape);   fprintf(gp, "e\n");
            drawShape2D(gp, result);  fprintf(gp, "e\n");
            fflush(gp);
            pclose(gp);
            return 0;
        }

        case 2: {
            // Oblique projection (Cavalier: scale=1, Cabinet: scale=0.5)
            int type;
            float angle, scale;
            printf("Oblique type:\n  1. Cavalier (scale=1)\n  2. Cabinet (scale=0.5)\nEnter (1 or 2): ");
            if (scanf("%d", &type) != 1) { printf("Invalid.\n"); return 1; }
            scale = (type == 2) ? 0.5f : 1.0f;
            printf("Enter projection angle (degrees, typically 30 or 45): ");
            if (scanf("%f", &angle) != 1) { printf("Invalid.\n"); return 1; }
            float r = angle * M_PI / 180;

            // Oblique matrix: x' = x + z*scale*cos(a), y' = y + z*scale*sin(a)
            float temp[DIM][DIM] = {
                {1, 0, 0, 0},
                {0, 1, 0, 0},
                {scale * cos(r), scale * sin(r), 0, 0},
                {0, 0, 0, 1}
            };

            multiply(shape, temp, result);

            fprintf(gp, "set xrange [-50:100]\nset yrange [-50:100]\n");
            fprintf(gp, "set xlabel 'X'\nset ylabel 'Y'\n");
            fprintf(gp, "set title '%s Oblique Projection (angle=%.1f deg)'\n",
                    type == 2 ? "Cabinet" : "Cavalier", angle);
            fprintf(gp, "plot '-' with lines title 'Original', "
                        "'-' with lines title 'Projected'\n");
            drawShape2D(gp, shape);   fprintf(gp, "e\n");
            drawShape2D(gp, result);  fprintf(gp, "e\n");
            fflush(gp);
            pclose(gp);
            return 0;
        }

        case 3: {
            // Perspective projection (1-point, center of projection on Z axis)
            float d;
            printf("Enter distance of projection plane from origin (e.g. 60): ");
            if (scanf("%f", &d) != 1 || d == 0) { printf("Invalid.\n"); return 1; }

            // Perspective matrix: w' = z/d, then divide x,y by w'
            float temp[DIM][DIM] = {
                {1, 0,     0, 0},
                {0, 1,     0, 0},
                {0, 0,     1, 1.0f/d},
                {0, 0,     0, 0}
            };

            multiply(shape, temp, result);
            perspectiveDivide(result, projected);

            fprintf(gp, "set xrange [-100:100]\nset yrange [-100:100]\n");
            fprintf(gp, "set xlabel 'X'\nset ylabel 'Y'\n");
            fprintf(gp, "set title '1-Point Perspective Projection (d=%.1f)'\n", d);
            fprintf(gp, "plot '-' with lines title 'Original (XY)', "
                        "'-' with lines title 'Perspective'\n");
            drawShape2D(gp, shape);      fprintf(gp, "e\n");
            drawShape2D(gp, projected);  fprintf(gp, "e\n");
            fflush(gp);
            pclose(gp);
            return 0;
        }

        case 4: {
            printf("\n[Isometric Projection]\n");
            float ry = -45.0f * M_PI / 180;
            float rx =  35.264f * M_PI / 180;

            float Ry[DIM][DIM] = {
                { cos(ry), 0, -sin(ry), 0},
                {       0, 1,        0, 0},
                { sin(ry), 0,  cos(ry), 0},
                {       0, 0,        0, 1}
            };
            float Rx[DIM][DIM] = {
                {1,      0,       0, 0},
                {0,  cos(rx),  sin(rx), 0},
                {0, -sin(rx),  cos(rx), 0},
                {0,      0,       0, 1}
            };

            float Pxy[DIM][DIM] = {
                {1, 0, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 1}
            };

            float temp1[N][DIM], temp2[N][DIM];
            multiply(shape, Ry, temp1);
            multiply(temp1, Rx, temp2);
            multiply(temp2, Pxy, result);

            fprintf(gp, "set xrange [-100:100]\nset yrange [-100:100]\n");
            fprintf(gp, "set xlabel 'X'\nset ylabel 'Y'\n");
            fprintf(gp, "set title 'Isometric Projection'\n");
            fprintf(gp, "plot '-' with lines title 'Original (XY)', "
                        "'-' with lines title 'Isometric'\n");
            drawShape2D(gp, shape);   fprintf(gp, "e\n");
            drawShape2D(gp, result);  fprintf(gp, "e\n");
            fflush(gp);
            pclose(gp);
            return 0;
        }

        default:
            printf("Invalid choice.\n");
            return 1;
    }
    multiply(shape, T, result);
    fprintf(gp, "set xrange [-150:150]\nset yrange [-150:150]\nset zrange [-150:150]\n");
    fprintf(gp, "set xlabel 'X'\nset ylabel 'Y'\nset zlabel 'Z'\n");
    fprintf(gp, "set ticslevel 0\nset view 60, 30\n");
    fprintf(gp, "splot '-' with lines title 'Original', '-' with lines title 'Transformed'\n");
    drawShape3D(gp, shape);   fprintf(gp, "e\n");
    drawShape3D(gp, result);  fprintf(gp, "e\n");
    fflush(gp);
    pclose(gp);
    return 0;
}