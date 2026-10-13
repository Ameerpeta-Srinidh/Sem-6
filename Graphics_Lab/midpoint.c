#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/* Image data */
unsigned char *image;
int width, height, channels;

/* Set pixel to red */
void setPixel(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height)
        return;

    int index = (y * width + x) * channels;
    image[index]     = 255; // R
    image[index + 1] = 0;   // G
    image[index + 2] = 0;   // B
}

/* 8-way symmetry */
void plotCircle(int xc, int yc, int x, int y) {
    setPixel(xc + x, yc + y);
    setPixel(xc - x, yc + y);
    setPixel(xc + x, yc - y);
    setPixel(xc - x, yc - y);
    setPixel(xc + y, yc + x);
    setPixel(xc - y, yc + x);
    setPixel(xc + y, yc - x);
    setPixel(xc - y, yc - x);
}

/* Midpoint Circle Algorithm */
void midpointCircle(int xc, int yc, int r) {
    int x = 0, y = r;
    int p = 1 - r;

    while (x <= y) {
        plotCircle(xc, yc, x, y);
        x++;

        if (p < 0)
            p += 2 * x + 1;
        else {
            y--;
            p += 2 * (x - y) + 1;
        }
    }
}

int main() {
    /* Load image */
    image = stbi_load("input.jpg", &width, &height, &channels, 3);
    if (!image) {
        printf("Error: Could not load image\n");
        return 1;
    }

    printf("Image loaded: %d x %d\n", width, height);

    int xc, yc, r;
    printf("Enter center (x y): ");
    scanf("%d %d", &xc, &yc);
    printf("Enter radius: ");
    scanf("%d", &r);

    midpointCircle(xc, yc, r);

    /* Save output image */
    stbi_write_png("output.png", width, height, 3, image, width * 3);

    stbi_image_free(image);

    printf("Circle drawn. Check output.png\n");
    return 0;
}
