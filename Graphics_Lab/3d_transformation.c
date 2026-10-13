#include <stdio.h>
#include <math.h>
#include <string.h>

#define N 4  
#define DIM 4 

void drawShape(FILE *gp, float shape[N][DIM]) {
    int edges[6][2] = {{0,1},{0,2},{0,3},{1,2},{1,3},{2,3}};
    for (int e = 0; e < 6; e++) {
        int i = edges[e][0], j = edges[e][1];
        fprintf(gp, "%f %f %f\n", shape[i][0], shape[i][1], shape[i][2]);
        fprintf(gp, "%f %f %f\n\n", shape[j][0], shape[j][1], shape[j][2]);
    }
}

void multiply(float a[N][DIM], float t[DIM][DIM], float result[N][DIM]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < DIM; j++) {
            result[i][j] = 0;
            for (int k = 0; k < DIM; k++)
                result[i][j] += a[i][k] * t[k][j];
        }
}

int main() {
    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) {
        fprintf(stderr, "Error: Could not open gnuplot.\n");
        return 1;
    }
    fprintf(gp, "set xrange [-150:150]\nset yrange [-150:150]\nset zrange [-150:150]\n");
    fprintf(gp, "set xlabel 'X'\nset ylabel 'Y'\nset zlabel 'Z'\n");
    fprintf(gp, "set ticslevel 0\nset view 60, 30\n");
    fprintf(gp, "splot '-' with lines title 'Original', '-' with lines title 'Transformed'\n");

    float shape[N][DIM] = {
        {10, 10, 10, 1},
        {30, 10, 10, 1},
        {20, 30, 10, 1},
        {20, 20, 30, 1}
    };

    float result[N][DIM];
    float T[DIM][DIM];
    int choice;

    printf("\nChoose a 3D transformation:\n");
    printf("  1. Translation\n");
    printf("  2. Scaling\n");
    printf("  3. Rotation (X axis)\n");
    printf("  4. Rotation (Y axis)\n");
    printf("  5. Rotation (Z axis)\n");
    printf("  6. Reflection\n");
    printf("  7. Shearing\n");
    printf("Enter choice (1-7): ");
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // Identity matrix helper
    memset(T, 0, sizeof(T));
    T[0][0] = T[1][1] = T[2][2] = T[3][3] = 1;

    switch (choice) {
        case 1: {
            float tx, ty, tz;
            printf("Enter translation values (tx ty tz): ");
            if (scanf("%f %f %f", &tx, &ty, &tz) != 3) { printf("Invalid.\n"); return 1; }
            float temp[DIM][DIM] = {
                {1,  0,  0,  0},
                {0,  1,  0,  0},
                {0,  0,  1,  0},
                {tx, ty, tz, 1}
            };
            memcpy(T, temp, sizeof(T));
            break;
        }
        case 2: {
            float sx, sy, sz;
            printf("Enter scaling factors (sx sy sz): ");
            if (scanf("%f %f %f", &sx, &sy, &sz) != 3) { printf("Invalid.\n"); return 1; }
            float temp[DIM][DIM] = {
                {sx, 0,  0,  0},
                {0,  sy, 0,  0},
                {0,  0,  sz, 0},
                {0,  0,  0,  1}
            };
            memcpy(T, temp, sizeof(T));
            break;
        }
        case 3: {
            float angle;
            printf("Enter rotation angle around X axis (degrees): ");
            if (scanf("%f", &angle) != 1) { printf("Invalid.\n"); return 1; }
            float r = angle * M_PI / 180;
            float temp[DIM][DIM] = {
                {1,      0,       0, 0},
                {0,  cos(r),  sin(r), 0},
                {0, -sin(r),  cos(r), 0},
                {0,      0,       0, 1}
            };
            memcpy(T, temp, sizeof(T));
            break;
        }
        case 4: {
            float angle;
            printf("Enter rotation angle around Y axis (degrees): ");
            if (scanf("%f", &angle) != 1) { printf("Invalid.\n"); return 1; }
            float r = angle * M_PI / 180;
            float temp[DIM][DIM] = {
                {cos(r),  0, -sin(r), 0},
                {0,       1,       0, 0},
                {sin(r),  0,  cos(r), 0},
                {0,       0,       0, 1}
            };
            memcpy(T, temp, sizeof(T));
            break;
        }
        case 5: {
            float angle;
            printf("Enter rotation angle around Z axis (degrees): ");
            if (scanf("%f", &angle) != 1) { printf("Invalid.\n"); return 1; }
            float r = angle * M_PI / 180;
            float temp[DIM][DIM] = {
                { cos(r), sin(r), 0, 0},
                {-sin(r), cos(r), 0, 0},
                {      0,      0, 1, 0},
                {      0,      0, 0, 1}
            };
            memcpy(T, temp, sizeof(T));
            break;
        }
        case 6: {
            int axis;
            printf("Reflection plane:\n  1. XY plane\n  2. YZ plane\n  3. XZ plane\nEnter (1-3): ");
            if (scanf("%d", &axis) != 1 || axis < 1 || axis > 3) { printf("Invalid.\n"); return 1; }
            // Negate Z for XY, X for YZ, Y for XZ
            if (axis == 1) T[2][2] = -1; // XY: flip Z
            if (axis == 2) T[0][0] = -1; // YZ: flip X
            if (axis == 3) T[1][1] = -1; // XZ: flip Y
            break;
        }
        case 7: {
            float sxy, sxz, syx, syz, szx, szy;
            printf("Enter shearing values (sxy sxz syx syz szx szy):\n");
            printf("  (use 0 for no shear on that component)\n");
            if (scanf("%f %f %f %f %f %f", &sxy, &sxz, &syx, &syz, &szx, &szy) != 6) {
                printf("Invalid.\n"); return 1;
            }
            float temp[DIM][DIM] = {
                {1,   sxy, sxz, 0},
                {syx, 1,   syz, 0},
                {szx, szy, 1,   0},
                {0,   0,   0,   1}
            };
            memcpy(T, temp, sizeof(T));
            break;
        }
        default:
            printf("Invalid choice.\n");
            return 1;
    }

    multiply(shape, T, result);
    drawShape(gp, shape);   fprintf(gp, "e\n");
    drawShape(gp, result);  fprintf(gp, "e\n");
    fflush(gp);
    pclose(gp);
    return 0;
}