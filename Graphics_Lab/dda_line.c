#include <stdio.h>
#include <math.h>

int main() {
    int x1 = 2, y1 = 3;
    int x2 = 15, y2 = 10;

    FILE *gp = popen("gnuplot -persistent", "w");

    fprintf(gp, "set xrange [0:20]\n");
    fprintf(gp, "set yrange [0:20]\n");
    fprintf(gp, "plot '-' with points pt 7\n");

    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    float xInc = dx / (float)steps;
    float yInc = dy / (float)steps;

    float x = x1;
    float y = y1;

    for(int i = 0; i <= steps; i++) {
        fprintf(gp, "%f %f\n", x, y);
        x += xInc;
        y += yInc;
    }

    fprintf(gp, "e\n");
    pclose(gp);

    return 0;
}
