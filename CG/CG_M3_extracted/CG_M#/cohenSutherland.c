#include <stdio.h>
#include <stdbool.h>

const int top=8;
const int bottom=4;
const int right=2;
const int left=1;

double x_min=2,y_min=2,x_max=5, y_max=4,z_min=0,z_max=0;

FILE* set_plot(double x1,double y1, double x2, double y2)
{
    FILE* gp = popen("gnuplot -persistent","w");
    fprintf(gp,"set title 'cohen-sutherland'\n");
    fprintf(gp,"set xrange[0:10]\n");
    fprintf(gp,"set yrange[0:10]\n");
    fprintf(gp,"set grid\n");

    fprintf(gp,"plot '-' with lines linewidth 2 linecolor rgb 'black' title 'window',"
                        "'-' with lines linewidth 2 linecolor rgb 'red' title 'original',"
                        "'-' with linespoints pointtype 7 pointsize 1.5 linewidth 3 linecolor rgb 'blue' title 'Clipped'\n");
    fprintf(gp,"%f %f\n%f %f\n%f %f\n%f %f\n%f %f\ne\n",x_min,y_min,x_min,y_max,x_max,y_max,x_max,y_min,x_min,y_min);

    fprintf(gp,"%f %f\n%f %f\ne\n",x1,y1,x2,y2);
    return gp;
}

void plot_line(FILE* gp,double x1,double y1,double x2,double y2)
{
    fprintf(gp,"%f %f\n%f %f\ne\n",x1,y1,x2,y2);
}

int compute_code(double x,double y)
{
    int code = 0;

    if (x < x_min) {
        code = code | left;
    } else if (x > x_max) {
        code = code | right;
    }

    if (y < y_min) {
        code = code | bottom;
    } else if (y > y_max) {
        code = code | top;
    }

    return code;
}

int choose_outside_point(int code1,int code2)
{
    if (code1 == 0) {
        return code2;
    } else {
        return code1;
    }
}


int main()
{
    double x1, y1, x2, y2;
    double x = 0.0, y = 0.0;

    printf("Enter x1 y1: ");
    if (scanf("%lf %lf", &x1, &y1) != 2) {
        return 1;
    }

    printf("Enter x2 y2: ");
    if (scanf("%lf %lf", &x2, &y2) != 2) {
        return 1;
    }

    FILE* f = set_plot(x1, y1, x2, y2);
    if (!f) {
        perror("gnuplot");
        return 1;
    }

    int accept = -1;
    int code1 = compute_code(x1, y1);
    int code2 = compute_code(x2, y2);

    while (true) {
        if ((code1 | code2) == 0) {
            accept = 0;
            break;
        } else if ((code1 & code2) != 0) {
            break;
        } else {
            int codeout = choose_outside_point(code1, code2);

            if (codeout & left) {
                y = y1 + ((y2 - y1) / (x2 - x1)) * (x_min - x1);
                x = x_min;
            } else if (codeout & right) {
                y = y1 + ((y2 - y1) / (x2 - x1)) * (x_max - x1);
                x = x_max;
            } else if (codeout & bottom) {
                x = x1 + ((x2 - x1) / (y2 - y1)) * (y_min - y1);
                y = y_min;
            } else if (codeout & top) {
                x = x1 + ((x2 - x1) / (y2 - y1)) * (y_max - y1);
                y = y_max;
            }

            if (codeout == code1) {
                x1 = x;
                y1 = y;
                code1 = compute_code(x, y);
            } else if (codeout == code2) {
                x2 = x;
                y2 = y;
                code2 = compute_code(x, y);
            }
        }
    }

    if (accept == 0) {
        plot_line(f, x1, y1, x2, y2);
    } else {
        printf("Line is rejected\n");
        fprintf(f, "e\n");
    }

    pclose(f);
    return 0;
}
