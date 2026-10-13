#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.141592653589793

int main() {
    FILE* gp = popen("gnuplot -persistent", "w");
    if (!gp) {
        perror("gnuplot");
        return EXIT_FAILURE;
    }

    // Set up gnuplot for 3D surface
    fprintf(gp, "set title 'Parametric Surface - Cylinder'\n");
    fprintf(gp, "set xlabel 'X'\n");
    fprintf(gp, "set ylabel 'Y'\n");
    fprintf(gp, "set zlabel 'Z'\n");
    fprintf(gp, "set hidden3d\n");
    fprintf(gp, "set view 60,30\n");
    fprintf(gp, "splot '-' with lines title 'Cylinder'\n");

    // Generate cylinder points
    double r = 4.0;
    double h = 5.0; // height
    int nu = 40; // number of u divisions (angle)
    int nv = 20; // number of v divisions (height)
    double du = 2 * PI / nu;
    double dv = 2 * h / nv;

    for (int i = 0; i <= nv; i++) {
        double v = -h + i * dv;
        for (int j = 0; j <= nu; j++) {
            double u = j * du;
            double x = r * cos(u);
            double y = r * sin(u);
            double z = v;
            fprintf(gp, "%f %f %f\n", x, y, z);
        }
        fprintf(gp, "\n"); // blank line to separate isolines
    }

    fprintf(gp, "e\n");
    pclose(gp);
    return EXIT_SUCCESS;
}