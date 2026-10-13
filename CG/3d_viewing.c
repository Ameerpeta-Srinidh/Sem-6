#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float x, y, z;
} Point3D;

int main() {
    int n, e;
    float d; // Distance to center of projection (Z = -d)

    printf("--- 3D Perspective Viewing Transformation ---\n");
    printf("Enter distance d to center of projection (Z = -d): ");
    if (scanf("%f", &d) != 1) return 1;

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

    fprintf(gp, "set title '3D Perspective Viewing (d = %f)'\n", d);
    fprintf(gp, "set size ratio -1\nset grid\n");
    fprintf(gp, "plot '-' with lines lw 2 lc rgb 'purple' title 'Perspective Projection'\n");

    for (int i = 0; i < e; i++) {
        int idx1 = edges[i][0];
        int idx2 = edges[i][1];

        // Perspective Projection formula: x' = x / (1 + z/d), y' = y / (1 + z/d)
        float denom1 = 1.0 + (v[idx1].z / d);
        float x1 = v[idx1].x / denom1;
        float y1 = v[idx1].y / denom1;
        
        float denom2 = 1.0 + (v[idx2].z / d);
        float x2 = v[idx2].x / denom2;
        float y2 = v[idx2].y / denom2;
        
        fprintf(gp, "%f %f\n%f %f\n\n", x1, y1, x2, y2);
    }
    fprintf(gp, "e\n");

    pclose(gp);
    free(v);
    free(edges);
    return 0;
}
