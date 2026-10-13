#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  double x1, y1, x2, y2;

  printf("Enter x1 y1: ");
  if (scanf("%lf %lf", &x1, &y1) != 2)
    return EXIT_FAILURE;

  printf("Enter x2 y2: ");
  if (scanf("%lf %lf", &x2, &y2) != 2)
    return EXIT_FAILURE;

  double dx = x2 - x1;
  double dy = y2 - y1;

  int steps = (int)ceil(fmax(fabs(dx), fabs(dy)));

  double x_inc = dx / steps;
  double y_inc = dy / steps;

  FILE *gp = popen("gnuplot -persistent", "w");
  if (!gp) {
    perror("gnuplot");
    return EXIT_FAILURE;
  }

  fprintf(gp,
          "set title 'DDA Line Drawing Algorithm'\n"
          "set xlabel 'X'\n"
          "set ylabel 'Y'\n"
          "set grid\n"
          "set size ratio -1\n"
          "set parametric\n"
          "set trange [0:1]\n"
          "plot "
          "%.2f + (%.2f - %.2f)*t, %.2f + (%.2f - %.2f)*t "
          "with lines lw 2 lc rgb 'blue' title 'Ideal Line', "
          "'-' with points pt 7 ps 2 lc rgb 'red' title 'DDA Pixels'\n",
          x1, x2, x1, y1, y2, y1);

  double x = x1, y = y1;
  for (int i = 0; i <= steps; i++) {
    fprintf(gp, "%d %d\n", (int)round(x), (int)round(y));
    x += x_inc;
    y += y_inc;
  }

  fprintf(gp, "e\n");

  fflush(gp);
  pclose(gp);
  return EXIT_SUCCESS;
}
