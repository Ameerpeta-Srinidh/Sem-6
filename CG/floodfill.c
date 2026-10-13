#include <stdio.h>
#include <stdlib.h>

// Simulated screen buffer
int screen[201][201] = {0};
int FILL_COLOR = 1;
int OLD_COLOR = 0;

void drawArrayBoundary(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        if (x1 >= 0 && x1 <= 200 && y1 >= 0 && y1 <= 200)
            screen[y1][x1] = 5; // Barrier
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

// Updated function to match the requested Flood Fill pseudocode
void FloodFill4(FILE *gp, int x, int y, int fillColor, int oldColor) {
    // Safety check to prevent array out-of-bounds errors
    if (x < 0 || x > 200 || y < 0 || y > 200) return;
    
    // 2: currentColor <- GetPixel(x, y)
    int currentColor = screen[y][x];
    
    // 3: if currentColor = oldColor then
    if (currentColor == oldColor) {
        
        // 4: SetPixel(x, y, fillColor)
        screen[y][x] = fillColor;
        fprintf(gp, "%d %d\n", x, y); // Sends to GNU Plot
        
        // 5 to 8: Recursive calls
        FloodFill4(gp, x + 1, y, fillColor, oldColor);
        FloodFill4(gp, x - 1, y, fillColor, oldColor);
        FloodFill4(gp, x, y + 1, fillColor, oldColor);
        FloodFill4(gp, x, y - 1, fillColor, oldColor);
    }
    // 9: end if
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

    for (int i = 0; i < n; i++) 
        drawArrayBoundary(x[i], y[i], x[(i + 1) % n], y[(i + 1) % n]);

    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) return 1;

    // Formatting for precise Cartesian plotting
    fprintf(gp, "set title 'Corrected Flood Fill Aligned'\n");
    fprintf(gp, "set xrange [0:200]\nset yrange [0:200]\n");
    fprintf(gp, "set size ratio -1\n"); // Keep 1:1 scale
    fprintf(gp, "set tics 10\n");      // Show grid every 10 units for alignment check
    fprintf(gp, "set grid\n");
    
    // Updated plot command for two data streams
    fprintf(gp, "plot '-' with points pt 5 ps 1 lc rgb 'red' title 'Flood Area', "
                "'-' with points pt 7 ps 1.5 lc rgb 'black' title 'Vertices'\n");
    
    // Passing the FILL_COLOR and OLD_COLOR variables to match the new signature
    FloodFill4(gp, sx, sy, FILL_COLOR, OLD_COLOR);
    fprintf(gp, "e\n"); // End stream 1 (Fill)

    // Send vertices to stream 2
    for (int i = 0; i < n; i++) {
        fprintf(gp, "%d %d\n", x[i], y[i]);
    }
    fprintf(gp, "e\n"); // End stream 2 (Vertices)

    pclose(gp);
    return 0;
}