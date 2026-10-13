#include <stdio.h>
#include <stdlib.h>


#include <limits.h>

// No image handling. We'll only compute points and plot them with gnuplot.
int width = 0, height = 0; // optional: used for plotting bounds if needed

FILE *outf = NULL;

void writePoint(int x, int y) {
    printf("%d %d\n", x, y);
    if (outf) fprintf(outf, "%d %d\n", x, y);
}

void plotCirclePoints(int xc, int yc, int x, int y) {
    writePoint(xc + x, yc + y);
    writePoint(xc - x, yc + y);
    writePoint(xc + x, yc - y);
    writePoint(xc - x, yc - y);
    writePoint(xc + y, yc + x);
    writePoint(xc - y, yc + x);
    writePoint(xc + y, yc - x);
    writePoint(xc - y, yc - x);
}

void bresenhamCircle(int xc, int yc, int r) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (x <= y) {
        plotCirclePoints(xc, yc, x, y);

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void finish_and_plot(void) {
    if (outf) fclose(outf);

    // Try gnuplot first
    int code = system("gnuplot --version >nul 2>&1");
    if (code == 0) {
        system("gnuplot -p -e \"set title 'Bresenham Circle (points)'; set size ratio -1; plot 'points.dat' with points pt 7 ps 1.5 lc rgb 'red'\"");
    } else {
        printf("Wrote points.dat. To view the plot: install gnuplot or run the following Python snippet:\n");
        printf("python - <<'PY'\nimport matplotlib.pyplot as plt\npts=[tuple(map(int,l.split())) for l in open('points.dat')]\nx=[p[0] for p in pts]; y=[p[1] for p in pts]\nplt.scatter(x,y,s=10)\nplt.gca().set_aspect('equal')\nplt.show()\nPY\n");
    }
}

int main(void) {
    int xc, yc, r;
    printf("Enter center (x y): ");
    if (scanf("%d %d", &xc, &yc) != 2) {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }
    printf("Enter radius: ");
    if (scanf("%d", &r) != 1 || r < 0) {
        fprintf(stderr, "Invalid radius\n");
        return 1;
    }

    outf = fopen("points.dat", "w");
    if (!outf) { perror("fopen"); return 1; }

    bresenhamCircle(xc, yc, r);

    // Close and try plotting
    finish_and_plot();

    return 0;
}
