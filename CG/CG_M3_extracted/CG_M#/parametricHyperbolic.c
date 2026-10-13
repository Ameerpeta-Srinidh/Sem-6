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
    fprintf(gp, "set title 'Parametric Surface - Hyperbolic Cylinder'\n");
    fprintf(gp, "set xlabel 'X'\n");
    fprintf(gp, "set ylabel 'Y'\n");
    fprintf(gp, "set zlabel 'Z'\n");
    fprintf(gp, "set hidden3d\n");
    fprintf(gp, "set view 60,30\n");
    fprintf(gp, "splot '-' with lines title 'Hyperbolic Cylinder'\n");

    // Generate hyperbolic cylinder points: x^2 - y^2 = 1
    // Parametric: x = cosh(u), y = sinh(u), z = v
    double u_min = -2.0;
    double u_max = 2.0;
    double v_min = -5.0;
    double v_max = 5.0;
    int nu = 20; // number of u divisions
    int nv = 20; // number of v divisions
    double du = (u_max - u_min) / nu;
    double dv = (v_max - v_min) / nv;

    for (int i = 0; i <= nu; i++) {
        double u = u_min + i * du;
        for (int j = 0; j <= nv; j++) {
            double v = v_min + j * dv;
            double x = cosh(u);
            double y = sinh(u);
            double z = v;
            fprintf(gp, "%f %f %f\n", x, y, z);
        }
        fprintf(gp, "\n"); // blank line to separate isolines
    }

    fprintf(gp, "e\n");
    pclose(gp);
    return EXIT_SUCCESS;
}