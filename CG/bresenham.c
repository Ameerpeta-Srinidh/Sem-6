#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int x1, y1, x2, y2;

  printf("Enter x1 y1: ");
  if (scanf("%d %d", &x1, &y1) != 2)
    return EXIT_FAILURE;

  printf("Enter x2 y2: ");
  if (scanf("%d %d", &x2, &y2) != 2)
    return EXIT_FAILURE;

  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);

  int sx = (x2 >= x1) ? 1 : -1;
  int sy = (y2 >= y1) ? 1 : -1;

  FILE *gp = popen("gnuplot -persistent", "w");
  if (!gp) {
    perror("gnuplot");
    return EXIT_FAILURE;
  }

  fprintf(gp,
          "set title 'Bresenham Line Drawing Algorithm'\n"
          "set xlabel 'X'\n"
          "set ylabel 'Y'\n"
          "set grid\n"
          "set size ratio -1\n"
          "set parametric\n"
          "set trange [0:1]\n"
          "plot "
          "%d + (%d - %d)*t, %d + (%d - %d)*t "
          "with lines lw 2 lc rgb 'blue' title 'Ideal Line', "
          "'-' with points pt 7 ps 2 lc rgb 'red' title 'Bresenham Pixels'\n",
          x1, x2, x1, y1, y2, y1);

  int x = x1;
  int y = y1;

  if (dx >= dy) {
    int p = 2 * dy - dx;
    for (int i = 0; i <= dx; i++) {
      fprintf(gp, "%d %d\n", x, y);
      x += sx;
      if (p >= 0) {
        y += sy;
        p -= 2 * dx;
      }
      p += 2 * dy;
    }
  } else {
    int p = 2 * dx - dy;
    for (int i = 0; i <= dy; i++) {
      fprintf(gp, "%d %d\n", x, y);
      y += sy;
      if (p >= 0) {
        x += sx;
        p -= 2 * dy;
      }
      p += 2 * dx;
    }
  }

  fprintf(gp, "e\n");

  fflush(gp);
  pclose(gp);
  return EXIT_SUCCESS;
}
