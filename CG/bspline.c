#include <process.h>
#include <stdio.h>
#include <stdlib.h>

#define GNUPLOT_PATH "\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" -persistent"

static double bspline_basis(
    int basis_index,
    int degree,
    double t,
    const double *knots,
    int last_basis_index,
    double end_knot
) {
    if (degree == 0) {
        if ((knots[basis_index] <= t && t < knots[basis_index + 1]) ||
            (t == end_knot && basis_index == last_basis_index)) {
            return 1.0;
        }
        return 0.0;
    }

    double left_denominator = knots[basis_index + degree] - knots[basis_index];
    double right_denominator = knots[basis_index + degree + 1] - knots[basis_index + 1];
    double left_term = 0.0;
    double right_term = 0.0;

    if (left_denominator != 0.0) {
        left_term =
            ((t - knots[basis_index]) / left_denominator) *
            bspline_basis(basis_index, degree - 1, t, knots, last_basis_index, end_knot);
    }

    if (right_denominator != 0.0) {
        right_term =
            ((knots[basis_index + degree + 1] - t) / right_denominator) *
            bspline_basis(basis_index + 1, degree - 1, t, knots, last_basis_index, end_knot);
    }

    return left_term + right_term;
}

static int is_non_decreasing(const double *knots, int count) {
    for (int i = 1; i < count; i++) {
        if (knots[i] < knots[i - 1]) {
            return 0;
        }
    }
    return 1;
}

static void print_knots(const double *knots, int count) {
    printf("T = { ");
    for (int i = 0; i < count; i++) {
        printf("%.3f", knots[i]);
        if (i + 1 < count) {
            printf(", ");
        }
    }
    printf(" }\n");
}

static void print_sample_values(
    int basis_count,
    int degree,
    const double *knots,
    int knot_count
) {
    double start = knots[0];
    double end = knots[knot_count - 1];
    int last_basis_index = basis_count - 1;

    printf("\nSample Basis Values:\n");
    for (int i = 0; i < basis_count; i++) {
        double t_mid = (knots[i] + knots[i + degree + 1]) / 2.0;
        if (t_mid < start) {
            t_mid = start;
        }
        if (t_mid > end) {
            t_mid = end;
        }

        printf("N[%d,%d](%.3f) = %.6f\n",
               i,
               degree,
               t_mid,
               bspline_basis(i, degree, t_mid, knots, last_basis_index, end));
    }
}

static int plot_basis_functions(
    int basis_count,
    int degree,
    const double *knots,
    int knot_count,
    int samples_per_span
) {
    const char *colors[] = {
        "purple", "dark-green", "blue", "brown", "red", "orange", "cyan", "black"
    };
    int color_count = (int)(sizeof(colors) / sizeof(colors[0]));
    int last_basis_index = basis_count - 1;
    int total_samples = (knot_count - 1) * samples_per_span;
    double start = knots[0];
    double end = knots[knot_count - 1];

    FILE *gp = _popen(GNUPLOT_PATH, "w");
    if (!gp) {
        printf("Error opening gnuplot.\n");
        return 0;
    }

    fprintf(gp, "set terminal windows\n");
    fprintf(gp, "set title 'B-Spline Basis Functions (All in One)'\n");
    fprintf(gp, "set xlabel 't'\n");
    fprintf(gp, "set ylabel 'N[i,k](t)'\n");
    fprintf(gp, "set xrange [%lf:%lf]\n", start, end);
    fprintf(gp, "set yrange [0:1.05]\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key right top\n");

    fprintf(gp, "plot ");
    for (int i = 0; i < basis_count; i++) {
        fprintf(gp,
                "'-' with lines lw 2 lc rgb '%s' title 'N[%d,%d](t)'",
                colors[i % color_count],
                i,
                degree);
        if (i + 1 < basis_count) {
            fprintf(gp, ", \\\n");
        } else {
            fprintf(gp, "\n");
        }
    }

    for (int i = 0; i < basis_count; i++) {
        for (int sample = 0; sample <= total_samples; sample++) {
            double alpha = (double)sample / (double)total_samples;
            double t = start + alpha * (end - start);
            double value = bspline_basis(i, degree, t, knots, last_basis_index, end);
            fprintf(gp, "%lf %lf\n", t, value);
        }
        fprintf(gp, "e\n");
    }

    fflush(gp);
    _pclose(gp);
    return 1;
}

int main(void) {
    int basis_count;
    int degree;
    int knot_count;
    int samples_per_span;
    double *knots = NULL;

    printf("--- B-Spline Basis Functions Using Cox-de Boor Recursion ---\n");
    printf("To reproduce the notebook-style graph, use: basis count = 4, degree = 2, knots = 0 1 2 3 4 5 6\n");

    printf("Enter number of basis functions: ");
    if (scanf("%d", &basis_count) != 1) {
        return 1;
    }

    if (basis_count < 1) {
        printf("Basis count must be at least 1.\n");
        return 1;
    }

    printf("Enter degree k: ");
    if (scanf("%d", &degree) != 1) {
        return 1;
    }

    if (degree < 0) {
        printf("Degree k must be non-negative.\n");
        return 1;
    }

    knot_count = basis_count + degree + 1;
    knots = (double *)malloc((size_t)knot_count * sizeof(double));
    if (!knots) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    printf("Enter %d knot values in non-decreasing order:\n", knot_count);
    for (int i = 0; i < knot_count; i++) {
        printf("t%d: ", i);
        if (scanf("%lf", &knots[i]) != 1) {
            free(knots);
            return 1;
        }
    }

    if (!is_non_decreasing(knots, knot_count)) {
        printf("Knot vector must be non-decreasing.\n");
        free(knots);
        return 1;
    }

    if (knots[0] == knots[knot_count - 1]) {
        printf("Invalid knot vector.\n");
        free(knots);
        return 1;
    }

    printf("Enter number of samples per knot span: ");
    if (scanf("%d", &samples_per_span) != 1) {
        free(knots);
        return 1;
    }

    if (samples_per_span < 10) {
        printf("Samples per span should be at least 10 for a smooth plot.\n");
        free(knots);
        return 1;
    }

    printf("\nKnot Vector:\n");
    print_knots(knots, knot_count);
    print_sample_values(basis_count, degree, knots, knot_count);

    if (!plot_basis_functions(basis_count, degree, knots, knot_count, samples_per_span)) {
        free(knots);
        return 1;
    }

    free(knots);
    return 0;
}
