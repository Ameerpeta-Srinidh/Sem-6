#include<stdio.h>
#include<math.h>
#include<stdlib.h>

#define MAX_POINTS 10

FILE* set_plot(int n,double p[][2])
{
    FILE* gp=popen("gnuplot -persistent","w");
    fprintf(gp,"set title 'bezier curve'\n");
    fprintf(gp,"set xrange[0:150]\n");
    fprintf(gp,"set yrange[0:150]\n");
    fprintf(gp,"set grid\n");

    fprintf(gp, "plot '-' with linespoints dt 2 pointsize 1.5 linewidth 2 linecolor rgb 'red' title 'Control Points', "
                    "'-' with linespoints pointtype 7 pointsize 1.5 linewidth 3 linecolor rgb 'blue' title 'Bezier Curve'\n");

    for(int i = 0; i <= n; i++) {
            fprintf(gp, "%f %f\n", p[i][0], p[i][1]);
    }
    fprintf(gp, "e\n");
    return gp;
}

void plot_curve(FILE* gp,double result[2])
{
    fprintf(gp, "%f %f\n", result[0], result[1]);
}

void de_casteljau(double points[][2],int n, double t,double result[2])
{
    double temp[MAX_POINTS][2];

    for (int i = 0; i <= n; i++) {
        temp[i][0] = points[i][0];
        temp[i][1] = points[i][1];
    }

    for (int k = 1; k <= n; k++) {
        for (int i = 0; i <= n - k; i++) {
            temp[i][0] = (1 - t) * temp[i][0] + t * temp[i + 1][0];
            temp[i][1] = (1 - t) * temp[i][1] + t * temp[i + 1][1];
        }
    }

    result[0] = temp[0][0];
    result[1] = temp[0][1];
}

int main()
{
    int n;
    double p[MAX_POINTS][2];
    double result[2];

    printf("Enter degree of bezier curve: ");
    if (scanf("%d", &n) != 1) {
        return EXIT_FAILURE;
    }

    if (n < 1 || n >= MAX_POINTS) {
        return EXIT_FAILURE;
    }

    printf("Enter %d control points (x y):\n", n + 1);
    for (int i = 0; i <= n; i++) {
        printf("P%d: ", i);
        if (scanf("%lf %lf", &p[i][0], &p[i][1]) != 2) {
            return EXIT_FAILURE;
        }
    }

    FILE* gp = set_plot(n, p);
    if (!gp) {
        perror("gnuplot");
        return EXIT_FAILURE;
    }

    for (double t = 0.0; t <= 1.0001; t += 0.01) {
        de_casteljau(p, n, t, result);
        plot_curve(gp, result);
    }

    fprintf(gp, "e\n");
    pclose(gp);
    return EXIT_SUCCESS;
}
