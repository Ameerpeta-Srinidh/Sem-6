#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void drawDDA(FILE *gp, double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    int steps = (int)ceil(fmax(fabs(dx), fabs(dy)));
    if (steps == 0) steps = 1;
    double x_inc = dx / steps;
    double y_inc = dy / steps;
    double x = x1, y = y1;
    for (int i = 0; i <= steps; i++) {
        fprintf(gp, "%d %d\n", (int)round(x), (int)round(y));
        x += x_inc;
        y += y_inc;
    }
    fprintf(gp, "e\n");
}

int main(void) {
    double x1, y1, x2, y2, shear_val;
    int choice;

    printf("Enter original x1 y1: ");
    if (scanf("%lf %lf", &x1, &y1) != 2) return EXIT_FAILURE;
    printf("Enter original x2 y2: ");
    if (scanf("%lf %lf", &x2, &y2) != 2) return EXIT_FAILURE;
    
    printf("Choose Shear Axis:\n1. X-Shear\n2. Y-Shear\nChoice: ");
    if (scanf("%d", &choice) != 1) return EXIT_FAILURE;
    printf("Enter shear value: ");
    if (scanf("%lf", &shear_val) != 1) return EXIT_FAILURE;

    double nx1 = x1, ny1 = y1, nx2 = x2, ny2 = y2;
    
    // Apply Shearing [cite: 174, 180]
    if (choice == 1) { 
        nx1 = x1 + shear_val * y1;
        nx2 = x2 + shear_val * y2;
    } else if (choice == 2) { 
        ny1 = y1 + shear_val * x1;
        ny2 = y2 + shear_val * x2;
    }

    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) { perror("gnuplot"); return EXIT_FAILURE; }

    fprintf(gp, "set title 'Shearing with Ideal Lines'\nset grid\nset size ratio -1\n");
    fprintf(gp, "set xzeroaxis lw 2 lc rgb 'black'\nset yzeroaxis lw 2 lc rgb 'black'\n");
    fprintf(gp, "set parametric\nset trange [0:1]\n");

    fprintf(gp, "plot %.2f + (%.2f - %.2f)*t, %.2f + (%.2f - %.2f)*t with lines lw 2 lc rgb 'gray' title 'Ideal Original', "
                "%.2f + (%.2f - %.2f)*t, %.2f + (%.2f - %.2f)*t with lines lw 2 lc rgb 'blue' title 'Ideal Transformed', "
                "'-' with points pt 7 ps 1.5 lc rgb 'black' title 'DDA Original', "
                "'-' with points pt 7 ps 1.5 lc rgb 'red' title 'DDA Transformed'\n",
            x1, x2, x1, y1, y2, y1,
            nx1, nx2, nx1, ny1, ny2, ny1);

    drawDDA(gp, x1, y1, x2, y2);
    drawDDA(gp, nx1, ny1, nx2, ny2);

    fflush(gp);
    pclose(gp);
    return EXIT_SUCCESS;
}