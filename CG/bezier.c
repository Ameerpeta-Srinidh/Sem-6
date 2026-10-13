#include <process.h>
#include <stdio.h>
#include <stdlib.h>

#define GNUPLOT_PATH "\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" -persistent"

typedef struct {
    double x;
    double y;
} Point2D;

static Point2D evaluate_bezier(const Point2D *control_points, int count, double t) {
    Point2D *temp = (Point2D *)malloc((size_t)count * sizeof(Point2D));
    Point2D point = {0.0, 0.0};

    if (!temp) {
        return point;
    }

    for (int i = 0; i < count; i++) {
        temp[i] = control_points[i];
    }

    for (int level = 1; level < count; level++) {
        for (int i = 0; i < count - level; i++) {
            temp[i].x = (1.0 - t) * temp[i].x + t * temp[i + 1].x;
            temp[i].y = (1.0 - t) * temp[i].y + t * temp[i + 1].y;
        }
    }

    point = temp[0];
    free(temp);
    return point;
}

static int generate_bezier_curve(
    const Point2D *control_points,
    int count,
    int samples,
    Point2D **curve_points,
    int *curve_count
) {
    *curve_count = samples + 1;
    *curve_points = (Point2D *)malloc((size_t)(*curve_count) * sizeof(Point2D));
    if (!*curve_points) {
        return 0;
    }

    for (int i = 0; i <= samples; i++) {
        double t = (double)i / (double)samples;
        (*curve_points)[i] = evaluate_bezier(control_points, count, t);
    }

    return 1;
}

static void print_points(const Point2D *points, int count) {
    for (int i = 0; i < count; i++) {
        printf("P%-3d = (%8.3f, %8.3f)\n", i, points[i].x, points[i].y);
    }
}

static int plot_curve(
    const Point2D *control_points,
    int control_count,
    const Point2D *curve_points,
    int curve_count
) {
    FILE *gp = _popen(GNUPLOT_PATH, "w");
    if (!gp) {
        printf("Error opening gnuplot.\n");
        return 0;
    }

    fprintf(gp, "set terminal windows\n");
    fprintf(gp, "set title 'Bezier Curve'\n");
    fprintf(gp, "set xlabel 'X'\n");
    fprintf(gp, "set ylabel 'Y'\n");
    fprintf(gp, "set size ratio -1\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "plot '-' with linespoints lw 1 lc rgb 'gray40' pt 7 ps 1.3 title 'Control Polygon', \\\n");
    fprintf(gp, "     '-' with lines lw 3 lc rgb 'red' title 'Bezier Curve'\n");

    for (int i = 0; i < control_count; i++) {
        fprintf(gp, "%lf %lf\n", control_points[i].x, control_points[i].y);
    }
    fprintf(gp, "e\n");

    for (int i = 0; i < curve_count; i++) {
        fprintf(gp, "%lf %lf\n", curve_points[i].x, curve_points[i].y);
    }
    fprintf(gp, "e\n");

    fflush(gp);
    _pclose(gp);
    return 1;
}

int main(void) {
    int control_count;
    int samples;
    Point2D *control_points = NULL;
    Point2D *curve_points = NULL;
    int curve_count = 0;

    printf("--- Bezier Curve ---\n");
    printf("Enter number of control points: ");
    if (scanf("%d", &control_count) != 1) {
        return 1;
    }

    if (control_count < 2) {
        printf("Bezier curve needs at least 2 control points.\n");
        return 1;
    }

    control_points = (Point2D *)malloc((size_t)control_count * sizeof(Point2D));
    if (!control_points) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    for (int i = 0; i < control_count; i++) {
        printf("Control point %d (x y): ", i);
        if (scanf("%lf %lf", &control_points[i].x, &control_points[i].y) != 2) {
            free(control_points);
            return 1;
        }
    }

    printf("Enter number of samples: ");
    if (scanf("%d", &samples) != 1) {
        free(control_points);
        return 1;
    }

    if (samples < 2) {
        printf("Samples must be at least 2.\n");
        free(control_points);
        return 1;
    }

    if (!generate_bezier_curve(control_points, control_count, samples, &curve_points, &curve_count)) {
        printf("Memory allocation failed.\n");
        free(control_points);
        return 1;
    }

    printf("\nControl Points:\n");
    print_points(control_points, control_count);

    printf("\nGenerated Curve Points:\n");
    print_points(curve_points, curve_count);

    plot_curve(control_points, control_count, curve_points, curve_count);

    free(control_points);
    free(curve_points);
    return 0;
}
