/* color_conversion.c */

#include "color_conversion.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>

/* Constants for RGB to YPbPr conversion */
#define R_COEFF 0.299
#define G_COEFF 0.587
#define B_COEFF 0.114

#define PB_R_COEFF -0.168736
#define PB_G_COEFF -0.331264
#define PB_B_COEFF 0.5

#define PR_R_COEFF 0.5
#define PR_G_COEFF -0.418688
#define PR_B_COEFF -0.081312

/* Constants for YPbPr to RGB conversion */
#define PR_TO_R_COEFF 1.402
#define PB_TO_G_COEFF -0.344136
#define PR_TO_G_COEFF -0.714136
#define PB_TO_B_COEFF 1.772

/* Helper function to clamp values between 0.0 and 1.0 */
static float clamp(float value, float min, float max);

/* Converts an RGB Image to a YPbPr Image */
YPbPr_image *rgb_to_ypbpr(Image *image)
{
    assert(image != NULL);

    int width = image->width;
    int height = image->height;

    /* Allocate memory for YPbPr_image */
    YPbPr_image *ypbpr_image = malloc(sizeof(YPbPr_image));
    assert(ypbpr_image != NULL);

    ypbpr_image->width = width;
    ypbpr_image->height = height;

    ypbpr_image->pixels = malloc(height * sizeof(YPbPr_pixel *));
    assert(ypbpr_image->pixels != NULL);

    for (int i = 0; i < height; i++) {
        ypbpr_image->pixels[i] = malloc(width * sizeof(YPbPr_pixel));
        assert(ypbpr_image->pixels[i] != NULL);
    }

    /* Convert each pixel from RGB to YPbPr */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Pixel rgb_pixel = image->pixels[y][x];

            /* Normalize RGB values to [0,1] */
            float r = rgb_pixel.red / 255.0;
            float g = rgb_pixel.green / 255.0;
            float b = rgb_pixel.blue / 255.0;

            /* Compute Y, Pb, Pr */
            float y_value = R_COEFF * r + G_COEFF * g + B_COEFF * b;
            float pb_value = PB_R_COEFF * r + PB_G_COEFF * g + PB_B_COEFF * b;
            float pr_value = PR_R_COEFF * r + PR_G_COEFF * g + PR_B_COEFF * b;

            /* Store the YPbPr values */
            ypbpr_image->pixels[y][x].y = y_value;
            ypbpr_image->pixels[y][x].pb = pb_value;
            ypbpr_image->pixels[y][x].pr = pr_value;
        }
    }

    return ypbpr_image;
}

/* Converts a YPbPr Image to an RGB Image */
Image *ypbpr_to_rgb(YPbPr_image *ypbpr_image)
{
    assert(ypbpr_image != NULL);

    int width = ypbpr_image->width;
    int height = ypbpr_image->height;

    /* Allocate memory for the RGB Image */
    Image *image = malloc(sizeof(Image));
    assert(image != NULL);

    image->width = width;
    image->height = height;

    image->pixels = malloc(height * sizeof(Pixel *));
    assert(image->pixels != NULL);

    for (int i = 0; i < height; i++) {
        image->pixels[i] = malloc(width * sizeof(Pixel));
        assert(image->pixels[i] != NULL);
    }

    /* Convert each pixel from YPbPr to RGB */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            YPbPr_pixel ypbpr_pixel = ypbpr_image->pixels[y][x];

            float y_value = ypbpr_pixel.y;
            float pb_value = ypbpr_pixel.pb;
            float pr_value = ypbpr_pixel.pr;

            /* Compute R, G, B */
            float r = y_value + PR_TO_R_COEFF * pr_value;
            float g = y_value + PB_TO_G_COEFF * pb_value + PR_TO_G_COEFF * pr_value;
            float b = y_value + PB_TO_B_COEFF * pb_value;

            /* Clamp values to [0,1] */
            r = clamp(r, 0.0, 1.0);
            g = clamp(g, 0.0, 1.0);
            b = clamp(b, 0.0, 1.0);

            /* Convert to [0,255] and store in Pixel */
            image->pixels[y][x].red = (uint8_t)(r * 255.0);
            image->pixels[y][x].green = (uint8_t)(g * 255.0);
            image->pixels[y][x].blue = (uint8_t)(b * 255.0);
        }
    }

    return image;
}

/* Frees the memory allocated for the YPbPr Image */
void free_ypbpr_image(YPbPr_image *ypbpr_image)
{
    if (ypbpr_image == NULL) {
        return;
    }

    for (int i = 0; i < ypbpr_image->height; i++) {
        free(ypbpr_image->pixels[i]);
    }
    free(ypbpr_image->pixels);
    free(ypbpr_image);
}

/* Helper function to clamp values between min and max */
static float clamp(float value, float min, float max)
{
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}
