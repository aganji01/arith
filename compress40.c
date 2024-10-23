/* compress40.c */

#include <stdio.h>
#include <stdlib.h>
#include "compress40.h"
#include <assert.h>

/* Include module headers */
#include "image_processing.h"
#include "color_conversion.h"
#include "chroma_processing.h"
#include "transform.h"
#include "quantization.h"
#include "io.h"

/* Compress40_compress function */
void compress40(FILE *input)
{
    /* 1. Image Reader and Preprocessor */
    Image *image = read_image(input);
    if (image == NULL) {
        fprintf(stderr, "Error: Failed to read image.\n");
        exit(EXIT_FAILURE);
    }
    Image *trimmed_image = trim_image(image);
    if (trimmed_image != image) {
        free_image(image);
    }

    /* 2. RGB to YPbPr Conversion */
    YPbPr_image *ypbpr_image = rgb_to_ypbpr(trimmed_image);
    free_image(trimmed_image);

    /* 3. Chroma Averaging and 2x2 Block Generation */
    Block_Array *block_array = create_blocks(ypbpr_image);
    free_ypbpr_image(ypbpr_image);

    /* 4. Discrete Cosine Transform (DCT) */
    DCT_Array *dct_array = perform_dct(block_array);
    free_block_array(block_array);

    /* 5. Quantization and Codeword Packaging */
    Codeword_Array *codeword_array = quantize_and_pack(dct_array);
    free_dct_array(dct_array);

    /* Calculate original image dimensions */
    int width = block_array->width * 2;    // Number of blocks horizontally * 2
    int height = block_array->height * 2;  // Number of blocks vertically * 2

    /* 6. Compressed Image Writer */
    write_compressed_image(stdout, codeword_array, width, height);
    free_codeword_array(codeword_array);
}

/* Decompress40_decompress function */
void decompress40(FILE *input)
{
    int width, height, codeword_count;

     /* 1. Compressed Image Reader */
    uint32_t *codewords = read_compressed_image(input, &width, &height, &codeword_count);
    if (codewords == NULL) {
        fprintf(stderr, "Error: Failed to read compressed image.\n");
        exit(EXIT_FAILURE);
    }

      /* Create Codeword_Array structure */
    Codeword_Array *codeword_array = malloc(sizeof(Codeword_Array));
    assert(codeword_array != NULL);
    codeword_array->count = codeword_count;
    codeword_array->words = codewords;

    /* 2. Codeword Unpackaging and Dequantization */
    DCT_Array *dct_array = unpack_and_dequantize(codeword_array);
    free_codeword_array(codeword_array);

    /* 3. Inverse Discrete Cosine Transform (IDCT) */
    Block_Array *block_array = perform_idct(dct_array);
    free_dct_array(dct_array);

    /* 4. Block Reassembly and Chroma Application */
    YPbPr_image *ypbpr_image = reassemble_blocks(block_array);
    free_block_array(block_array);

    /* 5. YPbPr to RGB Conversion */
    Image *image = ypbpr_to_rgb(ypbpr_image);
    free_ypbpr_image(ypbpr_image);

    /* 6. Image Writer */
    write_image(stdout, image);
    free_image(image);
    free_codeword_array(codeword_array);
}
