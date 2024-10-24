/* io.c */

#include "io.h"
#include "uarray2.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pnm.h>

/* Magic number for the compressed image format */
#define COMPRESSED_MAGIC_NUMBER "COMP40 Compressed image format 2\n"

/* Writes the compressed image data to the output file */
void write_compressed_image(FILE *output, Codeword_Array *codeword_array, int width, int height)
{
    assert(output != NULL);
    assert(codeword_array != NULL);

    /* Write the magic number */
    fprintf(output, "%s", COMPRESSED_MAGIC_NUMBER);

    /* Write the dimensions */
    fprintf(output, "%d %d\n", width, height);

    /* Write the codewords in big-endian order */
    int num_codewords = codeword_array->count;
    for (int i = 0; i < num_codewords; i++) {
        uint32_t codeword = codeword_array->words[i];

        /* Write each byte in big-endian order */
        for (int byte = 3; byte >= 0; byte--) {
            fputc((codeword >> (byte * 8)) & 0xFF, output);
        }
    }
}

/* Writes the Image data to the output file in PPM format */
void write_image(FILE *output, Image *image)
{
    assert(output != NULL);
    assert(image != NULL);

    /* Create a Pnm_ppm structure */
    Pnm_ppm pixmap = malloc(sizeof(*pixmap));
    assert(pixmap != NULL);

    pixmap->width = image->width;
    pixmap->height = image->height;
    pixmap->denominator = 255;

    /* Allocate memory for the pixel array */
    pixmap->pixels = UArray2_new(pixmap->width, pixmap->height, sizeof(struct Pnm_rgb));
    assert(pixmap->pixels != NULL);

    /* Copy pixel data */
    for (size_t y = 0; y < pixmap->height; y++) {
        for (size_t x = 0; x < pixmap->width; x++) {
            struct Pnm_rgb *pixel = UArray2_at(pixmap->pixels, x, y);
            Pixel img_pixel = image->pixels[y][x];

            pixel->red = img_pixel.red;
            pixel->green = img_pixel.green;
            pixel->blue = img_pixel.blue;
        }
    }

    /* Write the PPM image to the output file */
    Pnm_ppmwrite(output, pixmap);

    /* Free the Pnm_ppm structure */
    Pnm_ppmfree(&pixmap);
}