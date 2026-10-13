#include <stdio.h>
#include <stdlib.h>

#define N 3

int main(void) {
    float px[N], py[N];

    printf("Enter 3 control points (x y):\n");
    for (int i = 0; i < N; i++) {
        if (scanf("%f %f", &px[i], &py[i]) != 2) {
            return EXIT_FAILURE;
        }
    }

    printf("\nQuadratic Bezier Equation:\n");
    printf("P(t) = (1 - t)^2 * P0 + 2*t*(1 - t) * P1 + t^2 * P2\n");

    float m[3][3] = {
        {1, -2, 1},
        {-2, 2, 0},
        {1, 0, 0}
    };

    printf("\nBezier Matrix:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%6.2f ", m[i][j]);
        }
        printf("\n");
    }

    FILE *curve = fopen("curve.dat", "w");
    FILE *control = fopen("control.dat", "w");
    FILE *gnuplot;

    if (!curve || !control) {
        perror("output file");
        if (curve) fclose(curve);
        if (control) fclose(control);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < N; i++) {
        fprintf(control, "%f %f\n", px[i], py[i]);
    }
    fclose(control);

    for (float t = 0.0f; t <= 1.0001f; t += 0.01f) {
        float x = (1.0f - t) * (1.0f - t) * px[0] +
                  2.0f * t * (1.0f - t) * px[1] +
                  t * t * px[2];
        float y = (1.0f - t) * (1.0f - t) * py[0] +
                  2.0f * t * (1.0f - t) * py[1] +
                  t * t * py[2];
        fprintf(curve, "%f %f\n", x, y);
    }
    fclose(curve);

    printf("\nCurve generated using equation.\n");
    return EXIT_SUCCESS;
}

