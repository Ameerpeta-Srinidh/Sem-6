#include <stdio.h>
#include <stdlib.h>

void drawEllipse(int xc, int yc, long a, long b) {
    // Open gnuplot pipe
    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) { perror("gnuplot"); return; }

    // 1. Setup Gnuplot Configuration
    fprintf(gp, 
        "set title 'Midpoint Ellipse Algorithm: Pixels vs Ideal Curve'\n"
        "set grid\n"
        "set size ratio -1\n"
        "set parametric\n"
        "set trange [0:2*pi]\n"
        "set xrange [%ld:%ld]\n"
        "set yrange [%ld:%ld]\n",
        xc - a - 10, xc + a + 10, yc - b - 10, yc + b + 10
    );

    // 2. Define Plot: Ideal line (parametric) and '-' for midpoint data
    fprintf(gp, 
        "plot %d + %ld*cos(t), %d + %ld*sin(t) with lines lw 2 lc rgb 'blue' title 'Ideal Ellipse', "
        "'-' with points pt 7 ps 1 lc rgb 'red' title 'Midpoint Pixels'\n",
        xc, a, yc, b
    );

    long x = 0;
    long y = b;
    long a2 = a * a;
    long b2 = b * b;

    // Region 1: Slope < -1
    double d1 = b2 - (a2 * b) + (0.25 * a2);
    long dx = 2 * b2 * x;
    long dy = 2 * a2 * y;

    while (dx < dy) {
        fprintf(gp, "%ld %ld\n%ld %ld\n%ld %ld\n%ld %ld\n", 
                xc + x, yc + y, xc - x, yc + y, xc + x, yc - y, xc - x, yc - y);
        if (d1 < 0) {
            x++;
            dx += (2 * b2);
            d1 += dx + b2;
        } else {
            x++;
            y--;
            dx += (2 * b2);
            dy -= (2 * a2);
            d1 += dx - dy + b2;
        }
    }

    // Region 2: Slope > -1
    double d2 = b2 * (x + 0.5) * (x + 0.5) + a2 * (y - 1) * (y - 1) - a2 * b2;

    while (y >= 0) {
        fprintf(gp, "%ld %ld\n%ld %ld\n%ld %ld\n%ld %ld\n", 
                xc + x, yc + y, xc - x, yc + y, xc + x, yc - y, xc - x, yc - y);
        if (d2 > 0) {
            y--;
            dy -= (2 * a2);
            d2 += a2 - dy;
        } else {
            y--;
            x++;
            dx += (2 * b2);
            dy -= (2 * a2);
            d2 += dx - dy + a2;
        }
    }

    // End of data
    fprintf(gp, "e\n");
    pclose(gp);
}

int main() {
    int xc, yc;
    long a, b;

    printf("Enter Center (xc yc): ");
    if(scanf("%d %d", &xc, &yc) != 2) return 1;
    printf("Enter Semi-Major (a) and Semi-Minor (b) axes: ");
    if(scanf("%ld %ld", &a, &b) != 2) return 1;

    drawEllipse(xc, yc, a, b);

    return 0;
}