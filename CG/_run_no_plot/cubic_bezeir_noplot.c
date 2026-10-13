#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float x;
    float y;
} Point;

static void print_matrix(const char *name, float m[4][4]) {
    printf("\n%s:\n", name);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%8.3f ", m[i][j]);
        }
        printf("\n");
    }
}

static Point interpolate(Point p1, Point p2, float t) {
    Point p;
    p.x = (1.0f - t) * p1.x + t * p2.x;
    p.y = (1.0f - t) * p1.y + t * p2.y;
    return p;
}

static Point de_casteljau(Point p0, Point p1, Point p2, Point p3, float t) {
    Point q0 = interpolate(p0, p1, t);
    Point q1 = interpolate(p1, p2, t);
    Point q2 = interpolate(p2, p3, t);
    Point r0 = interpolate(q0, q1, t);
    Point r1 = interpolate(q1, q2, t);
    Point p = interpolate(r0, r1, t);

    printf("t=%.2f\n", t);
    printf("Level1: Q0(%.2f,%.2f) Q1(%.2f,%.2f) Q2(%.2f,%.2f)\n",
           q0.x, q0.y, q1.x, q1.y, q2.x, q2.y);
    printf("Level2: R0(%.2f,%.2f) R1(%.2f,%.2f)\n", r0.x, r0.y, r1.x, r1.y);
    printf("Final P(t)=(%.2f, %.2f)\n\n", p.x, p.y);

    return p;
}

static Point matrix_bezier(Point p0, Point p1, Point p2, Point p3, float t) {
    float tt[4] = {t * t * t, t * t, t, 1.0f};
    float mb[4][4] = {
        {-1, 3, -3, 1},
        {3, -6, 3, 0},
        {-3, 3, 0, 0},
        {1, 0, 0, 0}
    };
    float gx[4] = {p0.x, p1.x, p2.x, p3.x};
    float gy[4] = {p0.y, p1.y, p2.y, p3.y};
    float temp[4] = {0, 0, 0, 0};
    Point p;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp[i] += tt[j] * mb[j][i];
        }
    }

    p.x = temp[0] * gx[0] + temp[1] * gx[1] + temp[2] * gx[2] + temp[3] * gx[3];
    p.y = temp[0] * gy[0] + temp[1] * gy[1] + temp[2] * gy[2] + temp[3] * gy[3];

    printf("Matrix Result P(t)=(%.2f, %.2f)\n\n", p.x, p.y);
    return p;
}

int main(void) {
    Point p0, p1, p2, p3;
    float mb[4][4] = {
        {-1, 3, -3, 1},
        {3, -6, 3, 0},
        {-3, 3, 0, 0},
        {1, 0, 0, 0}
    };

    printf("Enter 4 control points (x y):\n");
    if (scanf("%f %f", &p0.x, &p0.y) != 2) return EXIT_FAILURE;
    if (scanf("%f %f", &p1.x, &p1.y) != 2) return EXIT_FAILURE;
    if (scanf("%f %f", &p2.x, &p2.y) != 2) return EXIT_FAILURE;
    if (scanf("%f %f", &p3.x, &p3.y) != 2) return EXIT_FAILURE;

    print_matrix("Cubic Bezier Basis Matrix (Mb)", mb);

    printf("\nGeometry Matrix (G):\n");
    printf("[ %.2f %.2f %.2f %.2f ] (X)\n", p0.x, p1.x, p2.x, p3.x);
    printf("[ %.2f %.2f %.2f %.2f ] (Y)\n\n", p0.y, p1.y, p2.y, p3.y);

    FILE *curve = fopen("curve.dat", "w");
    if (!curve) {
        perror("curve.dat");
        return EXIT_FAILURE;
    }

    printf("\n--- PROCESS OUTPUT ---\n");
    for (float t = 0.0f; t <= 1.0001f; t += 0.1f) {
        printf("====================================\n");
        Point p_a = de_casteljau(p0, p1, p2, p3, t);
        (void)matrix_bezier(p0, p1, p2, p3, t);
        fprintf(curve, "%f %f\n", p_a.x, p_a.y);
    }
    fclose(curve);

    FILE *control = fopen("control.dat", "w");
    if (!control) {
        perror("control.dat");
        return EXIT_FAILURE;
    }
    fprintf(control, "%f %f\n%f %f\n%f %f\n%f %f\n",
            p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
    fclose(control);
    return EXIT_SUCCESS;
}

