#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265

typedef struct {
    float x, y, z;
} Point3D;

int main() {
    int n, e;

    printf("--- Isometric Projection ---\n");
    printf("Enter number of vertices: ");
    if (scanf("%d", &n) != 1) return 1;

    Point3D *v = malloc(n * sizeof(Point3D));
    for (int i = 0; i < n; i++) {
        printf("Vertex %d (x y z): ", i);
        scanf("%f %f %f", &v[i].x, &v[i].y, &v[i].z);
    }

    printf("Enter number of edges: ");
    if (scanf("%d", &e) != 1) return 1;

    int (*edges)[2] = malloc(e * sizeof(int[2]));
    for (int i = 0; i < e; i++) {
        printf("Edge %d (index1 index2): ", i + 1);
        scanf("%d %d", &edges[i][0], &edges[i][1]);
    }

    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) {
        printf("Error opening gnuplot.\n");
        free(v);
        free(edges);
        return 1;
    }

    fprintf(gp, "set title 'Isometric Projection'\n");
    fprintf(gp, "set size ratio -1\nset grid\n");
    fprintf(gp, "plot '-' with lines lw 2 lc rgb 'blue' title '3D Isometric View'\n");

    // Standard Isometric Projection Formulas
    // Rotation by 45 deg on Y axis, and 35.264 deg on X axis
    // Equivalent transformations:
    // x' = (x - z) * cos(30 deg)
    // y' = y * cos(30 deg) - (x + z) * sin(30 deg)
    float cos30 = cos(30.0 * PI / 180.0);
    float sin30 = sin(30.0 * PI / 180.0);

    for (int i = 0; i < e; i++) {
        int idx1 = edges[i][0];
        int idx2 = edges[i][1];

        float x1 = (v[idx1].x - v[idx1].z) * cos30;
        float y1 = v[idx1].y * cos30 - (v[idx1].x + v[idx1].z) * sin30;
        
        float x2 = (v[idx2].x - v[idx2].z) * cos30;
        float y2 = v[idx2].y * cos30 - (v[idx2].x + v[idx2].z) * sin30;
        
        fprintf(gp, "%f %f\n%f %f\n\n", x1, y1, x2, y2);
    }
    fprintf(gp, "e\n");

    pclose(gp);
    free(v);
    free(edges);
    return 0;
}
