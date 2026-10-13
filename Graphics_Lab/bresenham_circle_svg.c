#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef struct { int x, y; } Point;

static Point *pts = NULL;
static int pts_count = 0;
static int pts_cap = 0;
static int minX, maxX, minY, maxY;

static void add_point(int x, int y) {
    if (pts_count >= pts_cap) {
        pts_cap = pts_cap ? pts_cap * 2 : 256;
        pts = realloc(pts, pts_cap * sizeof(Point));
        if (!pts) { perror("realloc"); exit(1); }
    }
    pts[pts_count].x = x;
    pts[pts_count].y = y;
    pts_count++;

    if (pts_count == 1) {
        minX = maxX = x; minY = maxY = y;
    } else {
        if (x < minX) minX = x;
        if (x > maxX) maxX = x;
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
    }
}

static void plot_symmetry(int xc, int yc, int x, int y) {
    add_point(xc + x, yc + y);
    add_point(xc - x, yc + y);
    add_point(xc + x, yc - y);
    add_point(xc - x, yc - y);
    add_point(xc + y, yc + x);
    add_point(xc - y, yc + x);
    add_point(xc + y, yc - x);
    add_point(xc - y, yc - x);
}

void bresenham_circle(int xc, int yc, int r) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    while (x <= y) {
        plot_symmetry(xc, yc, x, y);
        if (d <= 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}
void write_svg(const char *filename) {
    if (pts_count == 0) return;

    const int margin = 10; // pixels
    int w = maxX - minX + margin * 2;
    int h = maxY - minY + margin * 2;

    FILE *f = fopen(filename, "w");
    if (!f) { perror("fopen"); return; }

    fprintf(f, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\">\n", w, h, w, h);
    fprintf(f, "  <rect width=\"100%%\" height=\"100%%\" fill=\"white\"/>\n");

    // draw axis-like grid (optional, similar to matplotlib look)
    fprintf(f, "  <g stroke=\"#ddd\" stroke-width=\"0.5\">\n");
    for (int gx = minX; gx <= maxX; gx += 10) {
        int sx = gx - minX + margin;
        fprintf(f, "    <line x1=\"%d\" y1=\"0\" x2=\"%d\" y2=\"%d\" />\n", sx, sx, h);
    }
    for (int gy = minY; gy <= maxY; gy += 10) {
        int sy = (maxY - gy) + margin; 
        fprintf(f, "    <line x1=\"0\" y1=\"%d\" x2=\"%d\" y2=\"%d\" />\n", sy, w, sy);
    }
    fprintf(f, "  </g>\n");

    // plot points
    fprintf(f, "  <g fill=\"red\" stroke=\"none\">\n");
    for (int i = 0; i < pts_count; ++i) {
        int sx = pts[i].x - minX + margin;
        int sy = (maxY - pts[i].y) + margin; // invert y
        fprintf(f, "    <circle cx=\"%d\" cy=\"%d\" r=\"1.5\" />\n", sx, sy);
    }
    fprintf(f, "  </g>\n");

    // border
    fprintf(f, "  <rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" fill=\"none\" stroke=\"black\" />\n", w, h);

    fprintf(f, "</svg>\n");
    fclose(f);
    printf("Wrote %s (open in browser)\n", filename);

    // Always write a local PNG rasterized from points (no external tools needed)
    int scale = 4; // scale factor for nicer resolution
    int W = (maxX - minX + 20) * scale;
    int H = (maxY - minY + 20) * scale;
    unsigned char *img = malloc(W * H * 3);
    if (img) {
        // white background
        for (int i = 0; i < W * H * 3; ++i) img[i] = 255;
        // draw points as filled circles
        int rpx = scale; // radius in pixels
        for (int pi = 0; pi < pts_count; ++pi) {
            int sx = (pts[pi].x - minX + 10) * scale;
            int sy = (maxY - pts[pi].y + 10) * scale;
            int xmin = sx - rpx; if (xmin < 0) xmin = 0;
            int xmax = sx + rpx; if (xmax >= W) xmax = W - 1;
            int ymin = sy - rpx; if (ymin < 0) ymin = 0;
            int ymax = sy + rpx; if (ymax >= H) ymax = H - 1;
            for (int y = ymin; y <= ymax; ++y) {
                for (int x = xmin; x <= xmax; ++x) {
                    int dx = x - sx; int dy = y - sy;
                    if (dx*dx + dy*dy <= rpx*rpx) {
                        int idx = (y * W + x) * 3;
                        img[idx] = 255; img[idx+1] = 0; img[idx+2] = 0;
                    }
                }
            }
        }
        if (stbi_write_png("circle.png", W, H, 3, img, W * 3))
            printf("Wrote circle.png (raster)\n");
        free(img);
    }

    /* Try to auto-convert to PNG using ImageMagick or Inkscape if available */
    if (system("magick -version >nul 2>&1") == 0) {
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "magick -density 300 %s %s.convert.png", filename, filename);
        if (system(cmd) == 0) printf("Wrote %s.convert.png (via ImageMagick)\n", filename);
    } else if (system("inkscape --version >nul 2>&1") == 0) {
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "inkscape %s --export-filename=%s.convert.png", filename, filename);
        if (system(cmd) == 0) printf("Wrote %s.convert.png (via Inkscape)\n", filename);
    } else if (system("python -c \"import cairosvg\" >nul 2>&1") == 0) {
        char cmd[1024];
        snprintf(cmd, sizeof(cmd), "python -c \"import cairosvg; cairosvg.svg2png(url='%s', write_to='%s.convert.png')\"", filename, filename);
        if (system(cmd) == 0) printf("Wrote %s.convert.png (via cairosvg)\n", filename);
    } else {
        printf("No external SVG->PNG converter found (optional). Built-in raster 'circle.png' created instead.\n");
    }
}

int main(int argc, char **argv) {
    int xc, yc, r;
    if (argc == 4) {
        xc = atoi(argv[1]); yc = atoi(argv[2]); r = atoi(argv[3]);
    } else {
        printf("Enter center (x y): ");
        if (scanf("%d %d", &xc, &yc) != 2) return 1;
        printf("Enter radius: ");
        if (scanf("%d", &r) != 1) return 1;
    }
    if (r < 0) { fprintf(stderr, "radius must be non-negative\n"); return 1; }

    bresenham_circle(xc, yc, r);

    write_svg("circle.svg");

    free(pts);
    return 0;
}
