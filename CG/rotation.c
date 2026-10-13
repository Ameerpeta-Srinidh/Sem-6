#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void drawDDA(FILE *gp, double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    int steps = (int)ceil(fmax(fabs(dx), fabs(dy)));
    if (steps == 0) steps = 1;
    double x_inc = dx / steps;
    double y_inc = dy / steps;
    double x = x1, y = y1;
    for (int i = 0; i <= steps; i++) {
        fprintf(gp, "%d %d\n", (int)round(x), (int)round(y));
        x += x_inc;
        y += y_inc;
    }
    fprintf(gp, "e\n");
}

int main(void) {
    double x1, y1, x2, y2, angle, h, k;

    printf("Enter original x1 y1: ");
    if (scanf("%lf %lf", &x1, &y1) != 2) return EXIT_FAILURE;
    printf("Enter original x2 y2: ");
    if (scanf("%lf %lf", &x2, &y2) != 2) return EXIT_FAILURE;
    printf("Enter rotation angle (degrees, counter-clockwise): ");
    if (scanf("%lf", &angle) != 1) return EXIT_FAILURE;
    printf("Enter rotation point h k: ");
    if (scanf("%lf %lf", &h, &k) != 2) return EXIT_FAILURE;

    double rad = angle * acos(-1.0) / 180.0;
    double nx1 = h + (x1 - h) * cos(rad) - (y1 - k) * sin(rad);
    double ny1 = k + (x1 - h) * sin(rad) + (y1 - k) * cos(rad);
    double nx2 = h + (x2 - h) * cos(rad) - (y2 - k) * sin(rad);
    double ny2 = k + (x2 - h) * sin(rad) + (y2 - k) * cos(rad);

    double min_x = fmin(fmin(x1, x2), fmin(nx1, nx2));
    double max_x = fmax(fmax(x1, x2), fmax(nx1, nx2));
    double min_y = fmin(fmin(y1, y2), fmin(ny1, ny2));
    double max_y = fmax(fmax(y1, y2), fmax(ny1, ny2));

    min_x = fmin(min_x, h);
    max_x = fmax(max_x, h);
    min_y = fmin(min_y, k);
    max_y = fmax(max_y, k);

    double pad_x = fmax(1.0, (max_x - min_x) * 0.2);
    double pad_y = fmax(1.0, (max_y - min_y) * 0.2);

    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) { perror("gnuplot"); return EXIT_FAILURE; }

    fprintf(gp, "set title 'Rotation of DDA Line About Arbitrary Point'\nset grid\nset size ratio -1\n");
    fprintf(gp, "set xzeroaxis lw 2 lc rgb 'black'\nset yzeroaxis lw 2 lc rgb 'black'\n");
    fprintf(gp, "set parametric\nset trange [0:1]\n");
    fprintf(gp, "set xrange [%.2f:%.2f]\n", min_x - pad_x, max_x + pad_x);
    fprintf(gp, "set yrange [%.2f:%.2f]\n", min_y - pad_y, max_y + pad_y);
    fprintf(gp, "set label 'Rotation Point' at %.2f,%.2f point pt 7 ps 1.5\n", h, k);

    fprintf(gp, "plot %.2f + (%.2f - %.2f)*t, %.2f + (%.2f - %.2f)*t with lines lw 2 lc rgb 'gray' title 'Ideal Original', "
                "%.2f + (%.2f - %.2f)*t, %.2f + (%.2f - %.2f)*t with lines lw 2 lc rgb 'blue' title 'Ideal Transformed', "
                "'-' with points pt 7 ps 1.5 lc rgb 'black' title 'DDA Original', "
                "'-' with points pt 7 ps 1.5 lc rgb 'red' title 'DDA Transformed'\n",
            x1, x2, x1, y1, y2, y1,
            nx1, nx2, nx1, ny1, ny2, ny1);

    drawDDA(gp, x1, y1, x2, y2);
    drawDDA(gp, nx1, ny1, nx2, ny2);

    fflush(gp);
    pclose(gp);
    return EXIT_SUCCESS;
}
