/* image_processing.h */

#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <stdio.h>
#include <stdint.h>

/* Structure to represent an RGB pixel */
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Pixel;

/* Structure to represent an image */
typedef struct {
    int width;
    int height;
    Pixel **pixels;
} Image;

/* Function Prototypes */

/**
 * Reads a PPM image from the input file.
 * @param input The input file pointer.
 * @return A pointer to the Image struct containing image data.
 */
Image *read_image(FILE *input);

/**
 * Trims the image to have even width and height by removing
 * the last row and/or column if necessary.
 * @param image The original Image pointer.
 * @return A new Image pointer with even dimensions.
 */
Image *trim_image(Image *image);

/**
 * Frees the memory allocated for the Image.
 * @param image The Image pointer to be freed.
 */
void free_image(Image *image);

/**
 * Reads the compressed image from the input file, including the header and codewords.
 * @param input The input file pointer.
 * @param width Pointer to store the image width.
 * @param height Pointer to store the image height.
 * @param codeword_count Pointer to store the number of codewords.
 * @return An array of 32-bit codewords.
 */
uint32_t *read_compressed_image(FILE *input, int *width, int *height, int *codeword_count);

#endif /* IMAGE_PROCESSING_H */
