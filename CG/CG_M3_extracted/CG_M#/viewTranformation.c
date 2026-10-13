#include <stdio.h>
#include <stdlib.h>

double w_x_min = 0.0, w_y_min = 0.0, w_x_max = 100.0, w_y_max = 100.0;
double v_u_min = 50.0, v_v_min = 50.0, v_u_max = 250.0, v_v_max = 200.0;

FILE* set_plot(double p[4][2])
{
    FILE *gp = popen("gnuplot -persistent", "w");

    fprintf(gp, "set title 'Window-to-Viewport Mapping'\n");
    fprintf(gp, "set xrange [-10:300]; set yrange [-10:250]\n");
    fprintf(gp, "set grid; set size ratio -1\n");

    fprintf(gp, "set object 1 rect from %f,%f to %f,%f fs empty border lc rgb 'red' dt 2\n", 
            w_x_min, w_y_min, w_x_max, w_y_max);
    fprintf(gp, "set object 2 rect from %f,%f to %f,%f fs empty border lc rgb 'blue' dt 2\n", 
            v_u_min, v_v_min, v_u_max, v_v_max);

    fprintf(gp, "plot '-' with lines lw 2 lc rgb 'black' title 'Original (World)', "
                "'-' with lines lw 2 lc rgb 'blue' title 'Mapped (Viewport)'\n");

    for(int i = 0; i < 4; i++) {
        fprintf(gp, "%f %f\n", p[i][0], p[i][1]);
    }
    fprintf(gp, "%f %f\n", p[0][0], p[0][1]);
    fprintf(gp, "e\n");
    return gp;
}

void plot_point(FILE* gp, double x, double y)
{
    fprintf(gp, "%f %f\n", x, y);
}

int main()
{
    double p[4][2];

    printf("Enter 4 polygon vertices (x y):\n");
    for (int i = 0; i < 4; i++) {
        printf("P%d: ", i);
        if (scanf("%lf %lf", &p[i][0], &p[i][1]) != 2) {
            return 1;
        }
    }

    FILE* f = set_plot(p);
    if (!f) {
        perror("gnuplot");
        return 1;
    }

    double sx = (v_u_max - v_u_min) / (w_x_max - w_x_min);
    double sy = (v_v_max - v_v_min) / (w_y_max - w_y_min);

    double x;
    double y;

    for (int i = 0; i < 4; i++) {
        x = v_u_min + sx * (p[i][0] - w_x_min);
        y = v_v_min + sy * (p[i][1] - w_y_min);
        plot_point(f, x, y);
    }

    double x1 = v_u_min + sx * (p[0][0] - w_x_min);
    double y1 = v_v_min + sy * (p[0][1] - w_y_min);
    plot_point(f, x1, y1);

    fprintf(f, "e\n");
    pclose(f);
    return 0;
}
