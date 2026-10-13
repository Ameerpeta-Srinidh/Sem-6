#include <stdio.h>
#include <stdlib.h>

FILE* set_plot(int n,double P[][2])
{
    FILE* gp = popen("gnuplot -persistent", "w");

    fprintf(gp, "set title 'B-Spline'\n");
    fprintf(gp, "set xrange [-2:5]\n");
    fprintf(gp, "set yrange [-2:5]\n");
    fprintf(gp, "set grid\n");

    fprintf(gp, "plot '-' with linespoints dt 2 pointsize 1.5 linewidth 2 linecolor rgb 'red' title 'Control Points', "
                "'-' with lines linewidth 3 linecolor rgb 'blue' title 'B-Spline Curve'\n");

    for(int i = 0; i < n; i++) {
        fprintf(gp, "%f %f\n", P[i][0], P[i][1]);
    }
    fprintf(gp, "e\n");
    return gp;
}

void plot_curve(FILE* gp,double x, double y)
{
    fprintf(gp, "%f %f\n", x, y);
}

double N(int i, int k, double t, double knots[]) 
{
    if (k == 1) {
        if (t >= knots[i] && t < knots[i + 1]) {
            return 1.0;
        }
        return 0.0;
    }

    double term1 = 0.0;
    double term2 = 0.0;
    double d1 = knots[i + k - 1] - knots[i];
    double d2 = knots[i + k] - knots[i + 1];

    if (d1 != 0) {
        term1 = ((t - knots[i]) / d1) * N(i, k - 1, t, knots);
    }

    if (d2 != 0) {
        term2 = ((knots[i + k] - t) / d2) * N(i + 1, k - 1, t, knots);
    }

    return term1 + term2;
}

int main() 
{
    int n = 3;
    int k = 2;
    double P[4][2];

    printf("Enter 4 control points (x y):\n");
    for (int i = 0; i <= n; i++) {
        printf("P%d: ", i);
        if (scanf("%lf %lf", &P[i][0], &P[i][1]) != 2) {
            return 1;
        }
    }

    double knots[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0};

    FILE* f = set_plot(n + 1, P);
    if (!f) {
        perror("gnuplot");
        return 1;
    }

    double ts = knots[k - 1];
    double te = knots[4];

    for (double t = ts; t <= te - 0.001; t = t + 0.1) {
        double cx = 0.0;
        double cy = 0.0;

        for (int i = 0; i <= n; i++) {
            double b = N(i, k, t, knots);
            cx = cx + P[i][0] * b;
            cy = cy + P[i][1] * b;
        }

        plot_curve(f, cx, cy);
    }

    fprintf(f, "e\n");
    pclose(f);
    return 0;
}
