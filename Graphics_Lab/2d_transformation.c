#include <stdio.h>
#include <math.h>
#include <string.h>

#define N 3

void drawShape(FILE *gp, float shape[N][3]) {
    for(int i = 0; i < N; i++)
        fprintf(gp, "%f %f\n", shape[i][0], shape[i][1]);

    fprintf(gp, "%f %f\n", shape[0][0], shape[0][1]);
}

void multiply(float a[N][3], float t[3][3], float result[N][3]) {
    for(int i = 0; i < N; i++)
        for(int j = 0; j < 3; j++) {
            result[i][j] = 0;
            for(int k = 0; k < 3; k++)
                result[i][j] += a[i][k] * t[k][j];
        }
}

int main() {
    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) {
        fprintf(stderr, "Error: Could not open gnuplot.\n");
        return 1;
    }

   
    fprintf(gp, "set xrange [-100:100]\nset yrange [-100:100]\nset size square\n");
    fprintf(gp, "plot '-' with lines title 'Original', '-' with lines title 'Transformed'\n");

    float tri[N][3] = {
        {10, 10, 1},
        {30, 10, 1},
        {20, 30, 1}
    };
    float result[N][3];
    float T[3][3];
    int choice;
    
    printf("\nChoose a transformation:\n");
    printf("  1. Translation\n  2. Scaling\n  3. Rotation\n  4. Reflection\n  5. Shearing\n");
    printf("Enter choice (1-5): ");
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    switch(choice) {
        case 1: {
            float tx, ty;
            printf("Enter translation values (tx ty): ");
            if (scanf("%f %f", &tx, &ty) != 2) {
                printf("Invalid input.\n");
                return 1;
            }
            float temp[3][3] = {
                {1, 0, 0},
                {0, 1, 0},
                {tx, ty, 1}
            };
            memcpy(T, temp, sizeof(T));
            break;
        }
        case 2: {
            float sx, sy;
            printf("Enter scaling factors (sx sy): ");
            if (scanf("%f %f", &sx, &sy) != 2) {
                printf("Invalid input.\n");
                return 1;
            }
            float temp[3][3] = {
                {sx, 0, 0},
                {0, sy, 0},
                {0, 0, 1}
            };
            memcpy(T, temp, sizeof(T));
            break;
        }
        case 3: {
            float angle;
            printf("Enter rotation angle (degrees): ");
            if (scanf("%f", &angle) != 1) {
                printf("Invalid input.\n");
                return 1;
            }
            float rad = angle * M_PI / 180;
            float temp[3][3] = {
                {cos(rad),  sin(rad), 0},
                {-sin(rad), cos(rad), 0},
                {0, 0, 1}
            };
            memcpy(T, temp, sizeof(T));
            break;
        }
        case 4: {
            int r;
            printf("Reflection axis:\n  1. X-axis\n  2. Y-axis\nEnter axis (1 or 2): ");
            if (scanf("%d", &r) != 1 || (r != 1 && r != 2)) {
                printf("Invalid input.\n");
                return 1;
            }
            float temp[3][3] = {
                {r == 2 ? -1 : 1, 0, 0},
                {0, r == 1 ? -1 : 1, 0},
                {0, 0, 1}
            };
            memcpy(T, temp, sizeof(T));
            break;
        }
        case 5: {
            float shx, shy;
            printf("Enter shearing values (shx shy): ");
            if (scanf("%f %f", &shx, &shy) != 2) {
                printf("Invalid input.\n");
                return 1;
            }
            float temp[3][3] = {
                {1, shy, 0},
                {shx, 1, 0},
                {0, 0, 1}
            };
            memcpy(T, temp, sizeof(T));
            break;
        }
        default:
            printf("Invalid choice.\n");
            return 1;
    }

    multiply(tri, T, result);
    drawShape(gp, tri); fprintf(gp, "e\n");
    drawShape(gp, result); fprintf(gp, "e\n");
    fflush(gp);
    pclose(gp);
    return 0;
}
