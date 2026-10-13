#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int ymax;
    float x;
    float dx; 
} Edge;

void drawScanLines(int x_verts[], int y_verts[], int n) {
    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) {
        printf("Error: Could not open pipe to gnuplot.\n");
        return;
    }

    // 1. Configuration
    fprintf(gp, "set title 'Extended Scan Line with Polygon Outline'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set autoscale fix\n");
    fprintf(gp, "set offsets 5, 5, 5, 5\n"); 
    fprintf(gp, "set size ratio -1\n");
    
    fprintf(gp, "plot '-' with lines lc rgb 'blue' lw 1 title 'Scan Lines', "
                "'-' with linespoints pt 7 ps 1.5 lc rgb 'red' lw 2 title 'Polygon Outline'\n");

    int ymin = y_verts[0], ymax_poly = y_verts[0];
    int xmin = x_verts[0], xmax_poly = x_verts[0];
    for (int i = 1; i < n; i++) {
        if (y_verts[i] < ymin) ymin = y_verts[i];
        if (y_verts[i] > ymax_poly) ymax_poly = y_verts[i];
        if (x_verts[i] < xmin) xmin = x_verts[i];
        if (x_verts[i] > xmax_poly) xmax_poly = x_verts[i];
    }

    Edge aet[20];
    int aet_count = 0;

    // 2. Scanline Loop
    for (int y = ymin; y <= ymax_poly; y++) {
        for (int i = 0; i < n; i++) {
            int next = (i + 1) % n;
            int v1y = y_verts[i], v2y = y_verts[next];
            int v1x = x_verts[i], v2x = x_verts[next];

            if ((v1y == y && v2y > y) || (v2y == y && v1y > y)) {
                aet[aet_count].ymax = (v1y > v2y) ? v1y : v2y;
                aet[aet_count].x = (v1y < v2y) ? (float)v1x : (float)v2x;
                aet[aet_count].dx = (v1y != v2y) ? (float)(v1x - v2x) / (v1y - v2y) : 0;
                aet_count++;
            }
        }

        for (int i = 0; i < aet_count; i++) {
            if (y >= aet[i].ymax) {
                for (int j = i; j < aet_count - 1; j++) aet[j] = aet[j + 1];
                aet_count--;
                i--; 
            }
        }

        if (aet_count > 0) {
            fprintf(gp, "%d %d\n%d %d\n\n", xmin, y, xmax_poly, y);
        }

        for (int i = 0; i < aet_count; i++) {
            aet[i].x += aet[i].dx;
        }
    }

    fprintf(gp, "e\n");

    // 3. Draw joined vertices (Polygon Outline)
    for (int i = 0; i <= n; i++) {
        fprintf(gp, "%d %d\n", x_verts[i % n], y_verts[i % n]);
    }
    fprintf(gp, "e\n"); 

    pclose(gp);
}

int main() {
    int n, x[20], y[20];
    printf("Enter number of vertices: ");
    if (scanf("%d", &n) != 1 || n < 3) return 1;

    for (int i = 0; i < n; i++) {
        printf("Vertex %d (x y): ", i + 1);
        scanf("%d %d", &x[i], &y[i]);
    }

    drawScanLines(x, y, n);
    return 0;
}