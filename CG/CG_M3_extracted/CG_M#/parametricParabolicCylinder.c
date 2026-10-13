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
    fprintf(gp, "set title 'Parametric Surface - Parabolic Cylinder'\n");
    fprintf(gp, "set xlabel 'X'\n");
    fprintf(gp, "set ylabel 'Y'\n");
    fprintf(gp, "set zlabel 'Z'\n");
    fprintf(gp, "set hidden3d\n");
    fprintf(gp, "set view 60,30\n");
    fprintf(gp, "splot '-' with lines title 'Parabolic Cylinder'\n");

    // Generate parabolic cylinder points: x = y^2 + z^2
    double range = 2.0;
    int nu = 20; // number of v divisions
    int nv = 20; // number of w divisions
    double du = 2 * range / nu;
    double dv = 2 * range / nv;

    for (int i = 0; i <= nu; i++) {
        double v = -range + i * du;
        for (int j = 0; j <= nv; j++) {
            double w = -range + j * dv;
            double x = v*v + w*w;
            double y = v;
            double z = w;
            fprintf(gp, "%f %f %f\n", x, y, z);
        }
        fprintf(gp, "\n"); // blank line to separate isolines
    }

    fprintf(gp, "e\n");
    pclose(gp);
    return EXIT_SUCCESS;
}