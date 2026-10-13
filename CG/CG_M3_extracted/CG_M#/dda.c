#include<stdio.h>
#include<math.h>
#include<stdlib.h>

FILE* set_plot()
{
    FILE* gp=popen("gnuplot -p","w");
    fprintf(gp,"set title 'dda'\n");
    fprintf(gp, "set multiplot\n");
    fprintf(gp, "set xrange [0:12]\n"); 
    fprintf(gp, "set yrange [0:10]\n");
    fprintf(gp,"set grid\n");
    return gp;
}

void plot_original_line(FILE* gp,int x1,int y1,int x2, int y2)
{
    fprintf(gp,"plot '-' with lines linewidth 2 linecolor rgb 'red' title 'original line'\n");
    fprintf(gp,"%d %d\n%d %d\ne\n",x1,y1,x2,y2);
}

void plot_point(FILE* gp,int x, int y)
{
    fprintf(gp,"plot '-' with points pointtype 7 pointsize 1.5 linecolor rgb 'blue' title 'dda points'\n");
    fprintf(gp,"%d %d\n",x,y);
}

int main()
{
    FILE* gp = set_plot();
    if (!gp) {
        perror("gnuplot");
        return EXIT_FAILURE;
    }

    int x1, x2, y1, y2;

    printf("Enter x1 y1: ");
    if (scanf("%d %d", &x1, &y1) != 2) {
        pclose(gp);
        return EXIT_FAILURE;
    }

    printf("Enter x2 y2: ");
    if (scanf("%d %d", &x2, &y2) != 2) {
        pclose(gp);
        return EXIT_FAILURE;
    }

    plot_original_line(gp, x1, y1, x2, y2);

    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps;

    if (abs(dx) > abs(dy)) {
        steps = abs(dx);
    } else {
        steps = abs(dy);
    }

    if (steps == 0) {
        plot_point(gp, x1, y1);
        fprintf(gp, "e\n");
        pclose(gp);
        return EXIT_SUCCESS;
    }

    double xinc = (double)dx / steps;
    double yinc = (double)dy / steps;

    double x = (double)x1;
    double y = (double)y1;

    plot_point(gp, (int)round(x), (int)round(y));

    for (int i = 0; i < steps; i++) {
        x = x + xinc;
        y = y + yinc;
        plot_point(gp, (int)round(x), (int)round(y));
    }

    fprintf(gp, "e\n");
    pclose(gp);
    return EXIT_SUCCESS;
}
