#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float x, y, z;
} Point3D;

int main() {
    int n, e;
    printf("--- Orthographic Projection ---\n");
    printf("Enter number of vertices: ");
    scanf("%d", &n);

    Point3D *v = malloc(n * sizeof(Point3D));
    for (int i = 0; i < n; i++) {
        printf("Vertex %d (x y z): ", i);
        scanf("%f %f %f", &v[i].x, &v[i].y, &v[i].z);
    }

    printf("Enter number of edges: ");
    scanf("%d", &e);
    int (*edges)[2] = malloc(e * sizeof(int[2]));
    for (int i = 0; i < e; i++) {
        printf("Edge %d (index1 index2): ", i + 1);
        scanf("%d %d", &edges[i][0], &edges[i][1]);
    }

    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) return 1;

    fprintf(gp, "set title 'User-Defined Orthographic Projection'\n");
    fprintf(gp, "set size ratio -1\nset grid\n");
    fprintf(gp, "set xlabel 'X'; set ylabel 'Y'\n");
    fprintf(gp, "plot '-' with lines lw 2 lc rgb 'blue' title 'Projected Shape'\n");

    for (int i = 0; i < e; i++) {
        // Orthographic mapping: x' = x, y' = y
        fprintf(gp, "%f %f\n", v[edges[i][0]].x, v[edges[i][0]].y);
        fprintf(gp, "%f %f\n\n", v[edges[i][1]].x, v[edges[i][1]].y);
    }
    fprintf(gp, "e\n");

    pclose(gp);
    free(v);
    free(edges);
    return 0;
}