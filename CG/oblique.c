#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265

typedef struct {
    float x, y, z;
} Point3D;

int main() {
    int n, e;
    float theta, L;

    printf("--- Oblique Projection ---\n");
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

    printf("Enter projection angle in degrees (e.g., 45): ");
    scanf("%f", &theta);
    printf("Enter depth factor L (1.0 for Cavalier, 0.5 for Cabinet): ");
    scanf("%f", &L);

    float angle_rad = theta * (PI / 180.0);

    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) return 1;

    fprintf(gp, "set title 'User-Defined Oblique Projection'\n");
    fprintf(gp, "set size ratio -1\nset grid\n");
    fprintf(gp, "plot '-' with lines lw 2 lc rgb 'red' title '3D Oblique View'\n");

    for (int i = 0; i < e; i++) {
        // Oblique Formula: 
        // x' = x + z * L * cos(theta)
        // y' = y + z * L * sin(theta)
        float x1 = v[edges[i][0]].x + v[edges[i][0]].z * L * cos(angle_rad);
        float y1 = v[edges[i][0]].y + v[edges[i][0]].z * L * sin(angle_rad);
        float x2 = v[edges[i][1]].x + v[edges[i][1]].z * L * cos(angle_rad);
        float y2 = v[edges[i][1]].y + v[edges[i][1]].z * L * sin(angle_rad);
        
        fprintf(gp, "%f %f\n%f %f\n\n", x1, y1, x2, y2);
    }
    fprintf(gp, "e\n");

    pclose(gp);
    free(v);
    free(edges);
    return 0;
}