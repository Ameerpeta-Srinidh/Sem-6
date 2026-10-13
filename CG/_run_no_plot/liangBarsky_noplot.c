#include<stdio.h>
#include<stdbool.h>

double x_min=10,y_min=10,x_max=50, y_max=50;

FILE* set_plot(double x1,double y1,double x2,double y2)
{

    FILE* gp=popen("gnuplot -persistent","w");
    fprintf(gp,"set title 'liang barsky 2d'\n");
    fprintf(gp,"set xrange[0:100]\n");
    fprintf(gp,"set yrange[0:100]\n");
    fprintf(gp,"set grid\n");

    fprintf(gp,"plot '-' with lines linewidth 2 linecolor rgb 'black' title 'window',"
                "'-' with lines linewidth 2 linecolor rgb 'red' title 'original line',"
                "'-' with linespoints pointtype 7 pointsize 2 linewidth 2 linecolor rgb 'blue' title 'clipped line'\n");

    fprintf(gp,"%f %f\n",x_min,y_min);
    fprintf(gp,"%f %f\n",x_min,y_max);
    fprintf(gp,"%f %f\n",x_max,y_max);
    fprintf(gp,"%f %f\n",x_max,y_min);
    fprintf(gp,"%f %f\n",x_min,y_min);
    fprintf(gp,"e\n");

    fprintf(gp,"%f %f\n%f %f\ne\n",x1,y1,x2,y2);

    return gp;
}

void plot_line(FILE* gp,double x1,double y1,double x2,double y2)
{
    fprintf(gp,"%f %f\n%f %f\ne\n",x1,y1,x2,y2);
}

int main()
{
    double x1, y1, x2, y2;

    printf("Enter x1 y1: ");
    if (scanf("%lf %lf", &x1, &y1) != 2) {
        return 1;
    }

    printf("Enter x2 y2: ");
    if (scanf("%lf %lf", &x2, &y2) != 2) {
        return 1;
    }

    double dx = x2 - x1;
    double dy = y2 - y1;
    double t1 = 0.0;
    double t2 = 1.0;
    bool accept = true;
    double p[4] = {-dx, dx, -dy, dy};
    double q[4] = {x1 - x_min, x_max - x1, y1 - y_min, y_max - y1};

    for (int k = 0; k < 4; k++) {
        if (p[k] == 0) {
            if (q[k] < 0) {
                accept = false;
            }
        } else {
            double t = q[k] / p[k];
            if (p[k] < 0) {
                if (t1 < t) {
                    t1 = t;
                }
            } else {
                if (t2 > t) {
                    t2 = t;
                }
            }
        }
    }

    double x11 = x1 + t1 * dx;
    double y11 = y1 + t1 * dy;
    double x12 = x1 + t2 * dx;
    double y12 = y1 + t2 * dy;

    if (accept && t1 <= t2) { printf("Clipped line: (%.2f, %.2f) to (%.2f, %.2f)\n", x11, y11, x12, y12); } else { printf("Line rejected: it lies completely outside the clipping window.\n"); }
    return 0;
}

