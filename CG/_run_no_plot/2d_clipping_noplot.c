#include <stdio.h>
#include <stdlib.h>
#include <process.h>

#define GNUPLOT_PATH "\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" -persistent"

typedef struct {
    float x;
    float y;
} Point2D;

enum {
    INSIDE = 0,
    LEFT = 1,
    RIGHT = 2,
    BOTTOM = 4,
    TOP = 8
};

static int compute_code(float x, float y, float xmin, float ymin, float xmax, float ymax) {
    int code = INSIDE;

    if (x < xmin) {
        code |= LEFT;
    } else if (x > xmax) {
        code |= RIGHT;
    }

    if (y < ymin) {
        code |= BOTTOM;
    } else if (y > ymax) {
        code |= TOP;
    }

    return code;
}

static int cohen_sutherland_clip(
    Point2D *p1,
    Point2D *p2,
    float xmin,
    float ymin,
    float xmax,
    float ymax
) {
    int code1 = compute_code(p1->x, p1->y, xmin, ymin, xmax, ymax);
    int code2 = compute_code(p2->x, p2->y, xmin, ymin, xmax, ymax);

    while (1) {
        if (!(code1 | code2)) {
            return 1;
        }

        if (code1 & code2) {
            return 0;
        }

        int code_out = code1 ? code1 : code2;
        float x = 0.0f;
        float y = 0.0f;

        if (code_out & TOP) {
            x = p1->x + (p2->x - p1->x) * (ymax - p1->y) / (p2->y - p1->y);
            y = ymax;
        } else if (code_out & BOTTOM) {
            x = p1->x + (p2->x - p1->x) * (ymin - p1->y) / (p2->y - p1->y);
            y = ymin;
        } else if (code_out & RIGHT) {
            y = p1->y + (p2->y - p1->y) * (xmax - p1->x) / (p2->x - p1->x);
            x = xmax;
        } else if (code_out & LEFT) {
            y = p1->y + (p2->y - p1->y) * (xmin - p1->x) / (p2->x - p1->x);
            x = xmin;
        }

        if (code_out == code1) {
            p1->x = x;
            p1->y = y;
            code1 = compute_code(p1->x, p1->y, xmin, ymin, xmax, ymax);
        } else {
            p2->x = x;
            p2->y = y;
            code2 = compute_code(p2->x, p2->y, xmin, ymin, xmax, ymax);
        }
    }
}

int main(void) {
    float xmin, ymin, xmax, ymax;
    Point2D original_p1, original_p2;
    Point2D clipped_p1, clipped_p2;

    printf("--- 2D Line Clipping using Cohen-Sutherland Algorithm ---\n");
    printf("Enter clipping window (xmin ymin xmax ymax): ");
    if (scanf("%f %f %f %f", &xmin, &ymin, &xmax, &ymax) != 4) {
        return 1;
    }

    if (xmin >= xmax || ymin >= ymax) {
        printf("Invalid clipping window.\n");
        return 1;
    }

    printf("Enter line start point (x1 y1): ");
    if (scanf("%f %f", &original_p1.x, &original_p1.y) != 2) {
        return 1;
    }

    printf("Enter line end point (x2 y2): ");
    if (scanf("%f %f", &original_p2.x, &original_p2.y) != 2) {
        return 1;
    }

    clipped_p1 = original_p1;
    clipped_p2 = original_p2;

    int accepted = cohen_sutherland_clip(&clipped_p1, &clipped_p2, xmin, ymin, xmax, ymax);

    if (accepted) {
        printf("Clipped line: (%.2f, %.2f) to (%.2f, %.2f)\n",
               clipped_p1.x, clipped_p1.y, clipped_p2.x, clipped_p2.y);
    } else {
        printf("Line rejected: it lies completely outside the clipping window.\n");
    }
    return 0;
}

