#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define N 8
#define PI 3.14159265358979323846

static void multiply(float t[4][4], float p[4], float r[4]) {
    for (int i = 0; i < 4; i++) {
        r[i] = 0.0f;
        for (int j = 0; j < 4; j++) {
            r[i] += t[i][j] * p[j];
        }
    }
}

int main(void) {
    float cube[N][4] = {
        {0, 0, 0, 1}, {4, 0, 0, 1}, {4, 4, 0, 1}, {0, 4, 0, 1},
        {0, 0, 4, 1}, {4, 0, 4, 1}, {4, 4, 4, 1}, {0, 4, 4, 1}
    };
    float result[N][4];
    float t[4][4];
    int choice;
    float tx, ty, tz, sx, sy, sz, angle, sh;

    printf("\n===== 3D Transformations =====\n");
    printf("1. Translation\n");
    printf("2. Rotation\n");
    printf("3. Scaling\n");
    printf("4. Reflection\n");
    printf("5. Shearing\n");
    printf("Enter choice: ");
    if (scanf("%d", &choice) != 1) {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            t[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }

    switch (choice) {
    case 1:
        printf("Enter tx ty tz: ");
        if (scanf("%f %f %f", &tx, &ty, &tz) != 3) {
            return EXIT_FAILURE;
        }
        t[0][3] = tx;
        t[1][3] = ty;
        t[2][3] = tz;
        break;

    case 2: {
        int axis;
        printf("Rotate about (1.X 2.Y 3.Z): ");
        if (scanf("%d", &axis) != 1) {
            return EXIT_FAILURE;
        }
        printf("Enter angle (degrees): ");
        if (scanf("%f", &angle) != 1) {
            return EXIT_FAILURE;
        }

        angle = angle * (float)PI / 180.0f;
        if (axis == 1) {
            t[1][1] = cosf(angle);
            t[1][2] = -sinf(angle);
            t[2][1] = sinf(angle);
            t[2][2] = cosf(angle);
        } else if (axis == 2) {
            t[0][0] = cosf(angle);
            t[0][2] = sinf(angle);
            t[2][0] = -sinf(angle);
            t[2][2] = cosf(angle);
        } else {
            t[0][0] = cosf(angle);
            t[0][1] = -sinf(angle);
            t[1][0] = sinf(angle);
            t[1][1] = cosf(angle);
        }
        break;
    }

    case 3:
        printf("Enter sx sy sz: ");
        if (scanf("%f %f %f", &sx, &sy, &sz) != 3) {
            return EXIT_FAILURE;
        }
        t[0][0] = sx;
        t[1][1] = sy;
        t[2][2] = sz;
        break;

    case 4: {
        int plane;
        printf("Reflect about (1.XY 2.YZ 3.ZX): ");
        if (scanf("%d", &plane) != 1) {
            return EXIT_FAILURE;
        }
        if (plane == 1) {
            t[2][2] = -1.0f;
        } else if (plane == 2) {
            t[0][0] = -1.0f;
        } else {
            t[1][1] = -1.0f;
        }
        break;
    }

    case 5:
        printf("Enter shear factor: ");
        if (scanf("%f", &sh) != 1) {
            return EXIT_FAILURE;
        }
        t[0][1] = sh;
        break;

    default:
        printf("Invalid choice\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < N; i++) {
        multiply(t, cube[i], result[i]);
    }

    FILE *fp = fopen("cube.dat", "w");
    if (!fp) {
        perror("cube.dat");
        return EXIT_FAILURE;
    }

    int edges[][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    for (int i = 0; i < 12; i++) {
        fprintf(fp, "%f %f %f\n", cube[edges[i][0]][0], cube[edges[i][0]][1], cube[edges[i][0]][2]);
        fprintf(fp, "%f %f %f\n\n", cube[edges[i][1]][0], cube[edges[i][1]][1], cube[edges[i][1]][2]);
    }
    fprintf(fp, "\n");
    for (int i = 0; i < 12; i++) {
        fprintf(fp, "%f %f %f\n", result[edges[i][0]][0], result[edges[i][0]][1], result[edges[i][0]][2]);
        fprintf(fp, "%f %f %f\n\n", result[edges[i][1]][0], result[edges[i][1]][1], result[edges[i][1]][2]);
    }
    fclose(fp);

    FILE *gp = fopen("3d.gnu", "w");
    if (!gp) {
        perror("3d.gnu");
        return EXIT_FAILURE;
    }

    fprintf(gp, "set xlabel 'X'\n");
    fprintf(gp, "set ylabel 'Y'\n");
    fprintf(gp, "set zlabel 'Z'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set view 60,30\n");
    fprintf(gp, "set xrange [-5:10]\n");
    fprintf(gp, "set yrange [-5:10]\n");
    fprintf(gp, "set zrange [-5:10]\n");

    switch (choice) {
    case 1: fprintf(gp, "set title '3D Translation'\n"); break;
    case 2: fprintf(gp, "set title '3D Rotation'\n"); break;
    case 3: fprintf(gp, "set title '3D Scaling'\n"); break;
    case 4: fprintf(gp, "set title '3D Reflection'\n"); break;
    case 5: fprintf(gp, "set title '3D Shearing'\n"); break;
    }

    fprintf(gp, "splot 'cube.dat' index 0 with lines lw 2 title 'Original Cube',\\\n");
    fprintf(gp, "      'cube.dat' index 1 with lines lw 2 title 'Transformed Cube'\n");
    fprintf(gp, "pause -1\n");
    fclose(gp);

    system("gnuplot 3d.gnu");
    return EXIT_SUCCESS;
}
