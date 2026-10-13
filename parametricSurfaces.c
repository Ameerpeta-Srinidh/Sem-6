#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.141592653589793
#define SURFACE_COUNT 8

typedef struct {
    double x;
    double y;
    double z;
} Point3D;

typedef void (*PlotFunction)(FILE *gp);

typedef struct {
    const char *equation;
    const char *parametric[3];
    PlotFunction plot;
} SurfaceOption;

void setup_gnuplot(FILE *gp, const char *equation) {
    fprintf(gp, "set title '%s'\n", equation);
    fprintf(gp, "set xlabel 'X'\n");
    fprintf(gp, "set ylabel 'Y'\n");
    fprintf(gp, "set zlabel 'Z'\n");
    fprintf(gp, "set hidden3d\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "unset key\n");
    fprintf(gp, "set view 60,30\n");
}

void generate_sphere(FILE *gp) {
    const double r = 4.0;
    const int nu = 20;
    const int nv = 40;
    const double du = PI / nu;
    const double dv = 2 * PI / nv;

    fprintf(gp, "splot '-' with lines linewidth 1.5 notitle\n");
    for (int i = 0; i <= nu; i++) {
        double u = i * du;
        for (int j = 0; j <= nv; j++) {
            double v = j * dv;
            double x = r * sin(u) * cos(v);
            double y = r * sin(u) * sin(v);
            double z = r * cos(u);
            fprintf(gp, "%f %f %f\n", x, y, z);
        }
        fprintf(gp, "\n");
    }
    fprintf(gp, "e\n");
}

void generate_cylinder(FILE *gp) {
    const double r = 4.0;
    const double h = 5.0;
    const int nu = 40;
    const int nv = 20;
    const double du = 2 * PI / nu;
    const double dv = 2 * h / nv;

    fprintf(gp, "splot '-' with lines linewidth 1.5 notitle\n");
    for (int i = 0; i <= nv; i++) {
        double v = -h + i * dv;
        for (int j = 0; j <= nu; j++) {
            double u = j * du;
            double x = r * cos(u);
            double y = r * sin(u);
            double z = v;
            fprintf(gp, "%f %f %f\n", x, y, z);
        }
        fprintf(gp, "\n");
    }
    fprintf(gp, "e\n");
}

void generate_hyperbolic_cylinder(FILE *gp) {
    const double u_min = -2.0;
    const double u_max = 2.0;
    const double v_min = -5.0;
    const double v_max = 5.0;
    const int nu = 20;
    const int nv = 20;
    const double du = (u_max - u_min) / nu;
    const double dv = (v_max - v_min) / nv;

    fprintf(gp, "splot '-' with lines linewidth 1.5 notitle\n");
    for (int i = 0; i <= nu; i++) {
        double u = u_min + i * du;
        for (int j = 0; j <= nv; j++) {
            double v = v_min + j * dv;
            double x = cosh(u);
            double y = sinh(u);
            double z = v;
            fprintf(gp, "%f %f %f\n", x, y, z);
        }
        fprintf(gp, "\n");
    }
    fprintf(gp, "e\n");
}

void generate_elliptic_paraboloid(FILE *gp) {
    const double range = 2.0;
    const int nu = 20;
    const int nv = 20;
    const double du = 2 * range / nu;
    const double dv = 2 * range / nv;

    fprintf(gp, "splot '-' with lines linewidth 1.5 notitle\n");
    for (int i = 0; i <= nu; i++) {
        double u = -range + i * du;
        for (int j = 0; j <= nv; j++) {
            double v = -range + j * dv;
            double x = u;
            double y = v;
            double z = u * u + v * v;
            fprintf(gp, "%f %f %f\n", x, y, z);
        }
        fprintf(gp, "\n");
    }
    fprintf(gp, "e\n");
}

void generate_hyperbolic_paraboloid(FILE *gp) {
    const double range = 2.0;
    const int nu = 20;
    const int nv = 20;
    const double du = 2 * range / nu;
    const double dv = 2 * range / nv;

    fprintf(gp, "splot '-' with lines linewidth 1.5 notitle\n");
    for (int i = 0; i <= nu; i++) {
        double u = -range + i * du;
        for (int j = 0; j <= nv; j++) {
            double v = -range + j * dv;
            double x = u;
            double y = v;
            double z = u * u - v * v;
            fprintf(gp, "%f %f %f\n", x, y, z);
        }
        fprintf(gp, "\n");
    }
    fprintf(gp, "e\n");
}

void generate_parabolic_cylinder(FILE *gp) {
    const double range = 2.0;
    const int nu = 20;
    const int nv = 20;
    const double du = 2 * range / nu;
    const double dv = 2 * range / nv;

    fprintf(gp, "splot '-' with lines linewidth 1.5 notitle\n");
    for (int i = 0; i <= nu; i++) {
        double u = -range + i * du;
        for (int j = 0; j <= nv; j++) {
            double v = -range + j * dv;
            double x = u * u;
            double y = u;
            double z = v;
            fprintf(gp, "%f %f %f\n", x, y, z);
        }
        fprintf(gp, "\n");
    }
    fprintf(gp, "e\n");
}

void generate_torus(FILE *gp) {
    const double R = 6.0;
    const double r = 2.0;
    const int nu = 20;
    const int nv = 40;
    const double du = 2 * PI / nu;
    const double dv = 2 * PI / nv;

    fprintf(gp, "splot '-' with lines linewidth 1.5 notitle\n");
    for (int i = 0; i <= nu; i++) {
        double u = i * du;
        for (int j = 0; j <= nv; j++) {
            double v = j * dv;
            double x = (R + r * cos(u)) * cos(v);
            double y = (R + r * cos(u)) * sin(v);
            double z = r * sin(u);
            fprintf(gp, "%f %f %f\n", x, y, z);
        }
        fprintf(gp, "\n");
    }
    fprintf(gp, "e\n");
}

double bspline_basis(
    int basis_index,
    int degree,
    double parameter,
    const double *knots,
    int last_basis_index,
    double end_knot
) {
    if (degree == 0) {
        if ((knots[basis_index] <= parameter && parameter < knots[basis_index + 1]) ||
            (parameter == end_knot && basis_index == last_basis_index)) {
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
            ((parameter - knots[basis_index]) / left_denominator) *
            bspline_basis(
                basis_index,
                degree - 1,
                parameter,
                knots,
                last_basis_index,
                end_knot
            );
    }

    if (right_denominator != 0.0) {
        right_term =
            ((knots[basis_index + degree + 1] - parameter) / right_denominator) *
            bspline_basis(
                basis_index + 1,
                degree - 1,
                parameter,
                knots,
                last_basis_index,
                end_knot
            );
    }

    return left_term + right_term;
}

Point3D evaluate_bspline_surface(
    const Point3D control_points[4][4],
    int u_degree,
    int v_degree,
    const double *u_knots,
    const double *v_knots,
    double u,
    double v
) {
    Point3D point = {0.0, 0.0, 0.0};
    const int last_u_basis_index = 3;
    const int last_v_basis_index = 3;
    const double u_end = u_knots[7];
    const double v_end = v_knots[7];

    for (int i = 0; i < 4; i++) {
        double u_basis = bspline_basis(i, u_degree, u, u_knots, last_u_basis_index, u_end);
        for (int j = 0; j < 4; j++) {
            double v_basis = bspline_basis(j, v_degree, v, v_knots, last_v_basis_index, v_end);
            double weight = u_basis * v_basis;

            point.x += weight * control_points[i][j].x;
            point.y += weight * control_points[i][j].y;
            point.z += weight * control_points[i][j].z;
        }
    }

    return point;
}

void generate_bspline_surface(FILE *gp) {
    static const Point3D control_points[4][4] = {
        {{-6.0, -6.0, 0.0}, {-2.0, -6.0, 2.0}, {2.0, -6.0, -2.0}, {6.0, -6.0, 0.0}},
        {{-6.0, -2.0, 1.0}, {-2.0, -2.0, 6.0}, {2.0, -2.0, 4.0}, {6.0, -2.0, 1.0}},
        {{-6.0,  2.0, -1.0}, {-2.0,  2.0, 4.0}, {2.0,  2.0, 6.0}, {6.0,  2.0, -1.0}},
        {{-6.0,  6.0, 0.0}, {-2.0,  6.0, 1.0}, {2.0,  6.0, -1.0}, {6.0,  6.0, 0.0}}
    };
    static const double u_knots[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
    static const double v_knots[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
    const int u_degree = 3;
    const int v_degree = 3;
    const int u_samples = 24;
    const int v_samples = 24;

    fprintf(gp, "set title 'B-Spline Surface Patch'\n");
    fprintf(gp, "set style line 1 lc rgb 'steelblue' lw 1.5\n");
    fprintf(gp, "set style line 2 lc rgb 'dark-orange' lw 1.0 pt 7 ps 1\n");
    fprintf(gp, "splot '-' with lines ls 1 title 'B-Spline Surface', \\\n");
    fprintf(gp, "      '-' with linespoints ls 2 title 'Control Net'\n");

    for (int i = 0; i <= u_samples; i++) {
        double u = (double)i / (double)u_samples;
        for (int j = 0; j <= v_samples; j++) {
            double v = (double)j / (double)v_samples;
            Point3D point = evaluate_bspline_surface(
                control_points,
                u_degree,
                v_degree,
                u_knots,
                v_knots,
                u,
                v
            );
            fprintf(gp, "%f %f %f\n", point.x, point.y, point.z);
        }
        fprintf(gp, "\n");
    }
    fprintf(gp, "e\n");

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            fprintf(
                gp,
                "%f %f %f\n",
                control_points[i][j].x,
                control_points[i][j].y,
                control_points[i][j].z
            );
        }
        fprintf(gp, "\n");
    }

    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            fprintf(
                gp,
                "%f %f %f\n",
                control_points[i][j].x,
                control_points[i][j].y,
                control_points[i][j].z
            );
        }
        fprintf(gp, "\n");
    }
    fprintf(gp, "e\n");
}

void print_surface_details(const SurfaceOption *surface) {
    printf("\nEquation: %s\n", surface->equation);
    printf("Parametric form:\n");
    printf("  %s\n", surface->parametric[0]);
    printf("  %s\n", surface->parametric[1]);
    printf("  %s\n", surface->parametric[2]);
}

void plot_surface(const SurfaceOption *surface) {
    FILE *gp = popen("gnuplot -persistent", "w");

    if (!gp) {
        perror("gnuplot");
        return;
    }

    setup_gnuplot(gp, surface->equation);
    surface->plot(gp);
    fflush(gp);
    pclose(gp);
}

int read_choice(void) {
    int choice;
    int result = scanf("%d", &choice);

    if (result != 1) {
        while (getchar() != '\n' && !feof(stdin)) {
        }
        return -1;
    }

    return choice;
}

int main(void) {
    SurfaceOption surfaces[SURFACE_COUNT] = {
        {
            "x^2 + y^2 + z^2 = r^2",
            {
                "x = r sin(u) cos(v)",
                "y = r sin(u) sin(v)",
                "z = r cos(u)"
            },
            generate_sphere
        },
        {
            "x^2 + y^2 = r^2",
            {
                "x = r cos(u)",
                "y = r sin(u)",
                "z = v"
            },
            generate_cylinder
        },
        {
            "x^2 - y^2 = 1",
            {
                "x = cosh(u)",
                "y = sinh(u)",
                "z = v"
            },
            generate_hyperbolic_cylinder
        },
        {
            "z = x^2 + y^2",
            {
                "x = u",
                "y = v",
                "z = u^2 + v^2"
            },
            generate_elliptic_paraboloid
        },
        {
            "z = x^2 - y^2",
            {
                "x = u",
                "y = v",
                "z = u^2 - v^2"
            },
            generate_hyperbolic_paraboloid
        },
        {
            "y^2 = x",
            {
                "x = u^2",
                "y = u",
                "z = v"
            },
            generate_parabolic_cylinder
        },
        {
            "(sqrt(x^2 + y^2) - R)^2 + z^2 = r^2",
            {
                "x = (R + r cos(u)) cos(v)",
                "y = (R + r cos(u)) sin(v)",
                "z = r sin(u)"
            },
            generate_torus
        },
        {
            "Cubic B-Spline Surface Patch",
            {
                "S(u,v) = sum_i sum_j N_i,3(u) M_j,3(v) P_ij",
                "u, v in [0, 1] with clamped knot vectors",
                "16 control points define the control net"
            },
            generate_bspline_surface
        }
    };

    while (1) {
        printf("\nMenu Driven Parametric Surface Plotter\n");
        printf("--------------------------------------\n");
        for (int i = 0; i < SURFACE_COUNT; i++) {
            printf("%d. %s\n", i + 1, surfaces[i].equation);
        }
        printf("0. Exit\n");
        printf("\nEnter your choice: ");

        int choice = read_choice();
        if (choice == 0) {
            printf("Exiting program.\n");
            break;
        }

        if (choice < 1 || choice > SURFACE_COUNT) {
            printf("Invalid choice. Please enter a number between 0 and %d.\n", SURFACE_COUNT);
            continue;
        }

        print_surface_details(&surfaces[choice - 1]);
        printf("Opening plot in gnuplot...\n");
        plot_surface(&surfaces[choice - 1]);
    }

    return EXIT_SUCCESS;
}
