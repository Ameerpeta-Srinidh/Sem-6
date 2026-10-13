#include <stdio.h>
#include <stdlib.h>

static FILE *gp;

static void plot_points(int h, int k, int x, int y) {
    fprintf(gp, "%d %d\n", h + x, k + y);
    fprintf(gp, "%d %d\n", h - x, k + y);
    fprintf(gp, "%d %d\n", h + x, k - y);
    fprintf(gp, "%d %d\n", h - x, k - y);
    fprintf(gp, "%d %d\n", h + y, k + x);
    fprintf(gp, "%d %d\n", h - y, k + x);
    fprintf(gp, "%d %d\n", h + y, k - x);
    fprintf(gp, "%d %d\n", h - y, k - x);
}

static void bresenham_circle(int h, int k, int r) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    int iter = 0;

    printf("\nIter\tX\tY\tD\tDecision\n");
    fprintf(gp, "plot '-' with points pt 7 title 'Circle Points'\n");

    while (x <= y) {
        printf("%d\t%d\t%d\t%d\t%s\n", iter, x, y, d, (d < 0) ? "E" : "SE");
        plot_points(h, k, x, y);
        x++;

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        iter++;
    }

    fprintf(gp, "e\n");
}

int main(void) {
    int h, k, r;

    printf("Enter center coordinates (h k): ");
    if (scanf("%d %d", &h, &k) != 2) {
        return EXIT_FAILURE;
    }

    printf("Enter radius: ");
    if (scanf("%d", &r) != 1 || r <= 0) {
        return EXIT_FAILURE;
    }

    gp = popen("gnuplot -persistent", "w");
    if (!gp) {
        perror("gnuplot");
        return EXIT_FAILURE;
    }

    fprintf(gp, "set title 'Bresenham Circle Algorithm'\n");
    fprintf(gp, "set size square\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set xrange [%d:%d]\n", h - r - 5, h + r + 5);
    fprintf(gp, "set yrange [%d:%d]\n", k - r - 5, k + r + 5);

    bresenham_circle(h, k, r);

    pclose(gp);
    return EXIT_SUCCESS;
}
