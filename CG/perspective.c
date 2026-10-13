#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float x, y, z;
} Point3D;

int main() {
    int n, e;
    float d;

    printf("--- Perspective Projection ---\n");
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

    printf("Enter distance from projection plane to center of projection (d): ");
    scanf("%f", &d);

    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) {
        printf("Error: Could not open gnuplot.\n");
        free(v);
        free(edges);
        return 1;
    }

    fprintf(gp, "set title 'User-Defined Perspective Projection'\n");
    fprintf(gp, "set size ratio -1\nset grid\n");
    fprintf(gp, "set xlabel 'X'; set ylabel 'Y'\n");
    fprintf(gp, "plot '-' with lines lw 2 lc rgb 'green' title 'Perspective View'\n");

    for (int i = 0; i < e; i++) {
        // Perspective Formula:
        // Assuming the projection plane is at z = 0 and Center of Projection (COP) is at z = -d.
        // x' = x / (1 + z/d) = (x * d) / (z + d)
        // y' = y / (1 + z/d) = (y * d) / (z + d)
        
        float z_offset1 = v[edges[i][0]].z + d;
        float z_offset2 = v[edges[i][1]].z + d;

        // Prevent division by zero if a point is exactly at the COP
        if (z_offset1 == 0.0f) z_offset1 = 0.0001f;
        if (z_offset2 == 0.0f) z_offset2 = 0.0001f;

        float x1 = (v[edges[i][0]].x * d) / z_offset1;
        float y1 = (v[edges[i][0]].y * d) / z_offset1;
        float x2 = (v[edges[i][1]].x * d) / z_offset2;
        float y2 = (v[edges[i][1]].y * d) / z_offset2;
        
        fprintf(gp, "%f %f\n%f %f\n\n", x1, y1, x2, y2);
    }
    fprintf(gp, "e\n");

    pclose(gp);
    free(v);
    free(edges);
    return 0;
}
