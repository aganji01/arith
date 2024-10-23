/* image_processing.c */

#include "image_processing.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pnm.h>
#include <pnmrdr.h>
#include <ctype.h>

/* Constants */
#define MAX_COLOR_VALUE 255
#define COMPRESSED_MAGIC_NUMBER "COMP40 Compressed image format 2\n"

/* Function Prototypes */
static void skip_whitespace(FILE *input);

/* Reads a PPM image from the input file */
Image *read_image(FILE *input)
{
    assert(input != NULL);

    /* Use the Pnmrdr library to read the PPM image */
    Pnmrdr_T reader = Pnmrdr_new(input);
    if (reader == NULL) {
        fprintf(stderr, "Error: Could not read PPM image.\n");
        return NULL;
    }

    Pnmrdr_mapdata data = Pnmrdr_data(reader);
    if (data.type != Pnmrdr_rgb) {
        fprintf(stderr, "Error: Input file is not a PPM image.\n");
        Pnmrdr_free(&reader);
        return NULL;
    }

    int width = data.width;
    int height = data.height;

    /* Allocate memory for the Image structure */
    Image *image = malloc(sizeof(Image));
    assert(image != NULL);

    image->width = width;
    image->height = height;

    /* Allocate memory for the pixel array */
    image->pixels = malloc(height * sizeof(Pixel *));
    assert(image->pixels != NULL);

    for (int i = 0; i < height; i++) {
        image->pixels[i] = malloc(width * sizeof(Pixel));
        assert(image->pixels[i] != NULL);
    }

  /* Read pixel data */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            skip_whitespace(input);
            unsigned int red = Pnmrdr_get(reader);
            skip_whitespace(input);
            unsigned int green = Pnmrdr_get(reader);
            skip_whitespace(input);
            unsigned int blue = Pnmrdr_get(reader);

            image->pixels[y][x].red = red;
            image->pixels[y][x].green = green;
            image->pixels[y][x].blue = blue;
        }
    }

    Pnmrdr_free(&reader);
    return image;
}

/* Trims the image to have even dimensions */
Image *trim_image(Image *image)
{
    assert(image != NULL);

    int width = image->width;
    int height = image->height;

    /* Calculate new dimensions */
    int new_width = (width % 2 == 0) ? width : width - 1;
    int new_height = (height % 2 == 0) ? height : height - 1;

    if (new_width == width && new_height == height) {
        /* Dimensions are already even, return the original image */
        return image;
    }

    /* Create a new image with trimmed dimensions */
    Image *trimmed_image = malloc(sizeof(Image));
    assert(trimmed_image != NULL);

    trimmed_image->width = new_width;
    trimmed_image->height = new_height;

    /* Allocate memory for the pixel array */
    trimmed_image->pixels = malloc(new_height * sizeof(Pixel *));
    assert(trimmed_image->pixels != NULL);

    for (int i = 0; i < new_height; i++) {
        trimmed_image->pixels[i] = malloc(new_width * sizeof(Pixel));
        assert(trimmed_image->pixels[i] != NULL);
    }

    /* Copy pixel data */
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            trimmed_image->pixels[y][x] = image->pixels[y][x];
        }
    }

    /* Free the original image */
    free_image(image);

    return trimmed_image;
}

/* Frees the memory allocated for the Image */
void free_image(Image *image)
{
    if (image == NULL) {
        return;
    }

    for (int i = 0; i < image->height; i++) {
        free(image->pixels[i]);
    }
    free(image->pixels);
    free(image);
}

/* Reads the compressed image header and codewords */
uint32_t *read_compressed_image(FILE *input, int *width, int *height, int *codeword_count)
{
    assert(input != NULL);
    assert(width != NULL);
    assert(height != NULL);
    assert(codeword_count != NULL);

    /* Read and validate the magic number */
    char magic_number[256];
    if (fgets(magic_number, sizeof(magic_number), input) == NULL) {
        fprintf(stderr, "Error: Could not read compressed image magic number.\n");
        return NULL;
    }

    if (strcmp(magic_number, COMPRESSED_MAGIC_NUMBER) != 0) {
        fprintf(stderr, "Error: Invalid compressed image format.\n");
        return NULL;
    }

    /* Read the image width and height */
    int read_items = fscanf(input, "%d %d", width, height);
    if (read_items != 2) {
        fprintf(stderr, "Error: Could not read compressed image dimensions.\n");
        return NULL;
    }

    /* Consume the newline character after dimensions */
    int c = fgetc(input);
    if (c != '\n') {
        ungetc(c, input);
    }

    /* Skip any additional whitespace */
    skip_whitespace(input);

    /* Calculate the number of codewords */
    int num_codewords = (*width * *height) / 4;
    *codeword_count = num_codewords;

    /* Allocate memory for the codewords */
    uint32_t *codewords = malloc(num_codewords * sizeof(uint32_t));
    assert(codewords != NULL);

    /* Read codewords in big-endian order */
    for (int i = 0; i < num_codewords; i++) {
        uint32_t codeword = 0;
        for (int j = 0; j < 4; j++) {
            int byte = fgetc(input);
            if (byte == EOF) {
                fprintf(stderr, "Error: Unexpected end of file while reading codewords.\n");
                free(codewords);
                return NULL;
            }
            codeword = (codeword << 8) | (uint8_t)byte;
        }
        codewords[i] = codeword;
    }

    return codewords;
}

/* Helper function to skip whitespace characters */
static void skip_whitespace(FILE *input)
{
    int c;
    while ((c = fgetc(input)) != EOF) {
        if (!isspace(c)) {
            ungetc(c, input);
            break;
        }
    }
}
