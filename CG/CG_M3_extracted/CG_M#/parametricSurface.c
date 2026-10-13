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
    fprintf(gp, "set title 'Parametric Surface - Sphere'\n");
    fprintf(gp, "set xlabel 'X'\n");
    fprintf(gp, "set ylabel 'Y'\n");
    fprintf(gp, "set zlabel 'Z'\n");
    fprintf(gp, "set hidden3d\n");
    fprintf(gp, "set view 60,30\n");
    fprintf(gp, "splot '-' with lines title 'Sphere'\n");

    // Generate sphere points
    double r = 4.0;
    int nu = 20; // number of u divisions
    int nv = 40; // number of v divisions
    double du = PI / nu;
    double dv = 2 * PI / nv;

    for (int i = 0; i <= nu; i++) {
        double u = i * du;
        for (int j = 0; j <= nv; j++) {
            double v = j * dv;
            double x = r * sin(u) * cos(v);
            double y = r * sin(u) * sin(v);
            double z = r * cos(u);
            fprintf(gp, "%f %f %f\n", x, y, z);
        }
        fprintf(gp, "\n"); // blank line to separate isolines
    }

    fprintf(gp, "e\n");
    pclose(gp);
    return EXIT_SUCCESS;
}