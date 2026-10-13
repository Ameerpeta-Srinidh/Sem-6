#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define OFFSET 400
#define MAX_SIZE 801

int screen[MAX_SIZE][MAX_SIZE] = {0};
int BORDER = 2;
int FILL = 1;

void clearScreen() {
    for(int i = 0; i < MAX_SIZE; i++) {
        for(int j = 0; j < MAX_SIZE; j++) {
            screen[i][j] = 0;
        }
    }
}

void drawArrayBoundary(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        // Apply OFFSET to allow negative coordinates
        int rx = x1 + OFFSET;
        int ry = y1 + OFFSET;
        
        if (rx >= 0 && rx < MAX_SIZE && ry >= 0 && ry < MAX_SIZE)
            screen[ry][rx] = BORDER; 
            
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void BoundaryFill4(FILE *gp, int x, int y) {
    // Apply OFFSET to check array memory safely
    int rx = x + OFFSET;
    int ry = y + OFFSET;
    
    if (rx < 0 || rx >= MAX_SIZE || ry < 0 || ry >= MAX_SIZE) return;
    
    if (screen[ry][rx] != BORDER && screen[ry][rx] != FILL) {
        screen[ry][rx] = FILL;
        fprintf(gp, "%d %d\n", x, y); // Still print the real mathematical (x,y) to gnuplot
        
        BoundaryFill4(gp, x + 1, y);
        BoundaryFill4(gp, x - 1, y);
        BoundaryFill4(gp, x, y + 1);
        BoundaryFill4(gp, x, y - 1);
    }
}

int main() {
    int n, x[20], y[20], sx, sy;
    int tx_arr[20], ty_arr[20], tsx, tsy;
    float angle;

    printf("Enter number of vertices: ");
    if (scanf("%d", &n) != 1) return 1;
    for (int i = 0; i < n; i++) {
        printf("Vertex %d (x y): ", i + 1);
        scanf("%d %d", &x[i], &y[i]);
    }
    printf("Enter seed point (x y): ");
    scanf("%d %d", &sx, &sy);
    
    printf("Enter rotation angle in degrees: ");
    scanf("%f", &angle);

    // Calculate Transformation
    float rad = angle * acos(-1.0) / 180.0;
    for (int i = 0; i < n; i++) {
        tx_arr[i] = (int)round(x[i] * cos(rad) - y[i] * sin(rad));
        ty_arr[i] = (int)round(x[i] * sin(rad) + y[i] * cos(rad));
    }
    tsx = (int)round(sx * cos(rad) - sy * sin(rad));
    tsy = (int)round(sx * sin(rad) + sy * cos(rad));

    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) return 1;

    fprintf(gp, "set title 'Negative-Safe Rotation (Original vs Transformed)'\n");
    // Expand ranges to easily see negative transformations
    fprintf(gp, "set xrange [-100:100]\nset yrange [-50:150]\n");
    fprintf(gp, "set size ratio -1\nset tics 20\nset grid\n");
    fprintf(gp, "set xzeroaxis lw 2 lc rgb 'black'\nset yzeroaxis lw 2 lc rgb 'black'\n");
    
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