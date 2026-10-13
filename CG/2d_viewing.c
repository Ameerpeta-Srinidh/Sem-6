#include <stdio.h>
#include <stdlib.h>
#include <process.h>

typedef struct {
    float x, y;
} Point2D;

#define GNUPLOT_PATH "\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" -persistent"

int main() {
    float xw_min, yw_min, xw_max, yw_max;
    float xv_min, yv_min, xv_max, yv_max;

    printf("--- 2D Viewing Transformation ---\n");
    printf("Enter Window boundaries (xw_min yw_min xw_max yw_max): ");
    if (scanf("%f %f %f %f", &xw_min, &yw_min, &xw_max, &yw_max) != 4) return 1;

    printf("Enter Viewport boundaries (xv_min yv_min xv_max yv_max): ");
    if (scanf("%f %f %f %f", &xv_min, &yv_min, &xv_max, &yv_max) != 4) return 1;

    int n;
    printf("Enter number of vertices in polygon: ");
    if (scanf("%d", &n) != 1) return 1;

    Point2D *w_points = malloc(n * sizeof(Point2D));
    Point2D *v_points = malloc(n * sizeof(Point2D));

    // Calculate scaling factors
    float sx = (xv_max - xv_min) / (xw_max - xw_min);
    float sy = (yv_max - yv_min) / (yw_max - yw_min);

    for (int i = 0; i < n; i++) {
        printf("Vertex %d (x y) in Window coordinates: ", i);
        scanf("%f %f", &w_points[i].x, &w_points[i].y);
        
        // Window to Viewport transformation
        v_points[i].x = xv_min + (w_points[i].x - xw_min) * sx;
        v_points[i].y = yv_min + (w_points[i].y - yw_min) * sy;
    }

    FILE *gp = _popen(GNUPLOT_PATH, "w");
    if (!gp) {
        printf("Error opening gnuplot.\n");
        free(w_points);
        free(v_points);
        return 1;
    }

    fprintf(gp, "set terminal windows\n");
    fprintf(gp, "set title '2D Window to Viewport Transformation'\n");
    fprintf(gp, "set size ratio -1\nset grid\n");
    fprintf(gp, "plot '-' with lines lw 2 lc rgb 'red' title 'Original Window Polygon', \\\n");
    fprintf(gp, "     '-' with lines lw 2 lc rgb 'blue' title 'Transformed Viewport Polygon', \\\n");
    
    // Plot borders for visual reference
    fprintf(gp, "     '-' with lines lw 1 lc rgb 'black' title 'Window Boundary', \\\n");
    fprintf(gp, "     '-' with lines lw 1 lc rgb 'black' title 'Viewport Boundary'\n");

    // 1. Plot Window Polygon
    for (int i = 0; i <= n; i++) {
        int idx = i % n;
        fprintf(gp, "%f %f\n", w_points[idx].x, w_points[idx].y);
    }
    fprintf(gp, "e\n");

    // 2. Plot Viewport Polygon
    for (int i = 0; i <= n; i++) {
        int idx = i % n;
        fprintf(gp, "%f %f\n", v_points[idx].x, v_points[idx].y);
    }
    fprintf(gp, "e\n");

    // 3. Plot Window Boundary
    fprintf(gp, "%f %f\n", xw_min, yw_min);
    fprintf(gp, "%f %f\n", xw_max, yw_min);
    fprintf(gp, "%f %f\n", xw_max, yw_max);
    fprintf(gp, "%f %f\n", xw_min, yw_max);
    fprintf(gp, "%f %f\n", xw_min, yw_min);
    fprintf(gp, "e\n");

    // 4. Plot Viewport Boundary
    fprintf(gp, "%f %f\n", xv_min, yv_min);
    fprintf(gp, "%f %f\n", xv_max, yv_min);
    fprintf(gp, "%f %f\n", xv_max, yv_max);
    fprintf(gp, "%f %f\n", xv_min, yv_max);
    fprintf(gp, "%f %f\n", xv_min, yv_min);
    fprintf(gp, "e\n");

    fflush(gp);
    _pclose(gp);
    free(w_points);
    free(v_points);
    
    printf("Transformation applied successfully and plotted.\n");
    return 0;
}
