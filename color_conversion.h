/* color_conversion.h */

#ifndef COLOR_CONVERSION_H
#define COLOR_CONVERSION_H

#include "image_processing.h"  // For the Image struct

/* Structure to represent a YPbPr pixel */
typedef struct {
    float y;
    float pb;
    float pr;
} YPbPr_pixel;

/* Structure to represent a YPbPr image */
typedef struct {
    int width;
    int height;
    YPbPr_pixel **pixels;
} YPbPr_image;

/* Function Prototypes */

/**
 * Converts an RGB Image to a YPbPr Image.
 * @param image The input RGB Image.
 * @return A pointer to the YPbPr Image.
 */
YPbPr_image *rgb_to_ypbpr(Image *image);

/**
 * Converts a YPbPr Image to an RGB Image.
 * @param ypbpr_image The input YPbPr Image.
 * @return A pointer to the RGB Image.
 */
Image *ypbpr_to_rgb(YPbPr_image *ypbpr_image);

/**
 * Frees the memory allocated for the YPbPr Image.
 * @param ypbpr_image The YPbPr Image to be freed.
 */
void free_ypbpr_image(YPbPr_image *ypbpr_image);

#endif /* COLOR_CONVERSION_H */