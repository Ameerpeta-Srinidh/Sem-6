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
    fprintf(gp, "set title 'Parametric Surface - Hyperbolic Paraboloid'\n");
    fprintf(gp, "set xlabel 'X'\n");
    fprintf(gp, "set ylabel 'Y'\n");
    fprintf(gp, "set zlabel 'Z'\n");
    fprintf(gp, "set hidden3d\n");
    fprintf(gp, "set view 60,30\n");
    fprintf(gp, "splot '-' with lines title 'Hyperbolic Paraboloid'\n");

    // Generate hyperbolic paraboloid points: z = x^2 - y^2
    double range = 2.0;
    int nu = 20; // number of u divisions
    int nv = 20; // number of v divisions
    double du = 2 * range / nu;
    double dv = 2 * range / nv;

    for (int i = 0; i <= nu; i++) {
        double u = -range + i * du;
        for (int j = 0; j <= nv; j++) {
            double v = -range + j * dv;
            double x = u;
            double y = v;
            double z = u*u - v*v;
            fprintf(gp, "%f %f %f\n", x, y, z);
        }
        fprintf(gp, "\n"); // blank line to separate isolines
    }

    fprintf(gp, "e\n");
    pclose(gp);
    return EXIT_SUCCESS;
}