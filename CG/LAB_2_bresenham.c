#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int xc, yc, r;

    printf("Enter circle center (xc yc): ");
    if (scanf("%d %d", &xc, &yc) != 2) return EXIT_FAILURE;

    printf("Enter radius: ");
    if (scanf("%d", &r) != 1 || r <= 0) return EXIT_FAILURE;

    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) { perror("gnuplot"); return EXIT_FAILURE; }

    // Set axis ranges for proper circle visualization
    fprintf(gp,
        "set title 'Bresenham Circle Algorithm'\n"
        "set xlabel 'X'\n"
        "set ylabel 'Y'\n"
        "set grid\n"
        "set size ratio -1\n"
        "set parametric\n"
        "set trange [0:2*pi]\n"
        "set xrange [%d:%d]\n"
        "set yrange [%d:%d]\n"
        "plot "
        "%d + %d*cos(t), %d + %d*sin(t) "
        "with lines lw 2 lc rgb 'blue' title 'Ideal Circle', "
        "'-' with points pt 7 ps 2 lc rgb 'red' title 'Bresenham Pixels'\n",
        xc - r - 2, xc + r + 2,
        yc - r - 2, yc + r + 2,
        xc, r, yc, r
    );

    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (x <= y) {
        fprintf(gp, "%d %d\n", xc + x, yc + y);
        fprintf(gp, "%d %d\n", xc - x, yc + y);
        fprintf(gp, "%d %d\n", xc + x, yc - y);
        fprintf(gp, "%d %d\n", xc - x, yc - y);
        fprintf(gp, "%d %d\n", xc + y, yc + x);
        fprintf(gp, "%d %d\n", xc - y, yc + x);
        fprintf(gp, "%d %d\n", xc + y, yc - x);
        fprintf(gp, "%d %d\n", xc - y, yc - x);

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }

    fprintf(gp, "e\n");
    fflush(gp);
    pclose(gp);
    return EXIT_SUCCESS;
}
