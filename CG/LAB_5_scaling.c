#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int screen[201][201] = {0};
int BORDER = 2;
int FILL = 1;

void clearScreen() {
    for(int i = 0; i < 201; i++) {
        for(int j = 0; j < 201; j++) {
            screen[i][j] = 0;
        }
    }
}

void drawArrayBoundary(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        if (x1 >= 0 && x1 <= 200 && y1 >= 0 && y1 <= 200)
            screen[y1][x1] = BORDER; 
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void BoundaryFill4(FILE *gp, int x, int y) {
    if (x < 0 || x > 200 || y < 0 || y > 200) return;
    if (screen[y][x] != BORDER && screen[y][x] != FILL) {
        screen[y][x] = FILL;
        fprintf(gp, "%d %d\n", x, y);
        BoundaryFill4(gp, x + 1, y);
        BoundaryFill4(gp, x - 1, y);
        BoundaryFill4(gp, x, y + 1);
        BoundaryFill4(gp, x, y - 1);
    }
}

int main() {
    int n, x[20], y[20], sx, sy;
    int tx_arr[20], ty_arr[20], tsx, tsy;
    float s_x, s_y;

    printf("Enter number of vertices: ");
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        printf("Vertex %d (x y): ", i + 1);
        scanf("%d %d", &x[i], &y[i]);
    }
    printf("Enter seed point (x y): ");
    scanf("%d %d", &sx, &sy);
    
    printf("Enter scaling factors (sx sy): ");
    scanf("%f %f", &s_x, &s_y);

    // Calculate Transformation
    for (int i = 0; i < n; i++) {
        tx_arr[i] = (int)round(x[i] * s_x);
        ty_arr[i] = (int)round(y[i] * s_y);
    }
    tsx = (int)round(sx * s_x);
    tsy = (int)round(sy * s_y);

    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) return 1;

    fprintf(gp, "set title 'Scaling (Original vs Transformed)'\n");
    fprintf(gp, "set xrange [0:200]\nset yrange [0:200]\n");
    fprintf(gp, "set size ratio -1\nset tics 10\nset grid\n");
    
    fprintf(gp, "plot '-' with points pt 5 ps 0.5 lc rgb 'gray' title 'Original Fill', "
                "'-' with points pt 7 ps 1.5 lc rgb 'black' title 'Original Vertices', "
                "'-' with points pt 5 ps 0.5 lc rgb 'blue' title 'Transformed Fill', "
                "'-' with points pt 7 ps 1.5 lc rgb 'red' title 'Transformed Vertices'\n");
    
    // 1. Original Fill
    for (int i = 0; i < n; i++) 
        drawArrayBoundary(x[i], y[i], x[(i + 1) % n], y[(i + 1) % n]);
    BoundaryFill4(gp, sx, sy);
    fprintf(gp, "e\n");

    // 2. Original Vertices
    for (int i = 0; i < n; i++) fprintf(gp, "%d %d\n", x[i], y[i]);
    fprintf(gp, "e\n");

    clearScreen();

    // 3. Transformed Fill
    for (int i = 0; i < n; i++) 
        drawArrayBoundary(tx_arr[i], ty_arr[i], tx_arr[(i + 1) % n], ty_arr[(i + 1) % n]);
    BoundaryFill4(gp, tsx, tsy);
    fprintf(gp, "e\n");

    // 4. Transformed Vertices
    for (int i = 0; i < n; i++) fprintf(gp, "%d %d\n", tx_arr[i], ty_arr[i]);
    fprintf(gp, "e\n");

    pclose(gp);
    return 0;
}