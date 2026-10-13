#include <stdio.h>
#include <stdlib.h>

int screen[201][201] = {0};
int BORDER1 = 2; // First boundary color
int BORDER2 = 3; // Second boundary color
int FILL = 1;

// Updated to accept the specific color for the edge
void drawArrayBoundary(int x1, int y1, int x2, int y2, int color) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        if (x1 >= 0 && x1 <= 200 && y1 >= 0 && y1 <= 200)
            screen[y1][x1] = color; 
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

// Updated to stop at either boundary color
void BoundaryFill4(FILE *gp, int x, int y, int fillColor, int bColor1, int bColor2) {
    if (x < 0 || x > 200 || y < 0 || y > 200) return;
    
    int currentColor = screen[y][x];
    
    // Check against BOTH boundary colors and the fill color
    if (currentColor != bColor1 && currentColor != bColor2 && currentColor != fillColor) {
        screen[y][x] = fillColor;
        fprintf(gp, "%d %d\n", x, y); // Stream to GNU Plot
        
        BoundaryFill4(gp, x + 1, y, fillColor, bColor1, bColor2);
        BoundaryFill4(gp, x - 1, y, fillColor, bColor1, bColor2);
        BoundaryFill4(gp, x, y + 1, fillColor, bColor1, bColor2);
        BoundaryFill4(gp, x, y - 1, fillColor, bColor1, bColor2);
    }
}

int main() {
    int n, x[20], y[20], sx, sy;
    printf("Enter number of vertices: ");
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        printf("Vertex %d (x y): ", i + 1);
        scanf("%d %d", &x[i], &y[i]);
    }
    printf("Enter seed point (x y): ");
    scanf("%d %d", &sx, &sy);

    // Draw boundaries, alternating between the two boundary colors
    for (int i = 0; i < n; i++) {
        int color = (i % 2 == 0) ? BORDER1 : BORDER2;
        drawArrayBoundary(x[i], y[i], x[(i + 1) % n], y[(i + 1) % n], color);
    }

    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) return 1;

    fprintf(gp, "set title 'Boundary Fill with Two Boundary Colors'\n");
    fprintf(gp, "set xrange [0:200]\nset yrange [0:200]\n");
    fprintf(gp, "set size ratio -1\n");
    fprintf(gp, "set tics 10\n");
    fprintf(gp, "set grid\n");
    
    // Define 3 data streams: Fill, Border 1, and Border 2
    fprintf(gp, "plot '-' with points pt 5 ps 1 lc rgb 'blue' title 'Filled Area', "
                "'-' with points pt 5 ps 1 lc rgb 'red' title 'Boundary 1', "
                "'-' with points pt 5 ps 1 lc rgb 'green' title 'Boundary 2'\n");
    
    // Stream 1: Execute the fill algorithm and send filled points
    BoundaryFill4(gp, sx, sy, FILL, BORDER1, BORDER2);
    fprintf(gp, "e\n");

    // Stream 2: Scan the array to plot the first boundary color (Red)
    for (int y_coord = 0; y_coord <= 200; y_coord++) {
        for (int x_coord = 0; x_coord <= 200; x_coord++) {
            if (screen[y_coord][x_coord] == BORDER1) {
                fprintf(gp, "%d %d\n", x_coord, y_coord);
            }
        }
    }
    fprintf(gp, "e\n");

    // Stream 3: Scan the array to plot the second boundary color (Green)
    for (int y_coord = 0; y_coord <= 200; y_coord++) {
        for (int x_coord = 0; x_coord <= 200; x_coord++) {
            if (screen[y_coord][x_coord] == BORDER2) {
                fprintf(gp, "%d %d\n", x_coord, y_coord);
            }
        }
    }
    fprintf(gp, "e\n");

    pclose(gp);
    return 0;
}