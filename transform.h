/* transform.h */

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "chroma_processing.h"  // For Block_Array and Block

/* Structure to represent the DCT coefficients for a block */
typedef struct {
    /* Quantized DCT coefficients */
    float a;
    float b;
    float c;
    float d;

    /* Averaged chrominance (Pb and Pr) values */
    float pb_avg;
    float pr_avg;
} DCT_Block;

/* Structure to represent an array of DCT blocks */
typedef struct {
    int width;   // Number of blocks horizontally
    int height;  // Number of blocks vertically
    DCT_Block **blocks;
} DCT_Array;

/* Function Prototypes */

/**
 * Performs the Discrete Cosine Transform on the Block_Array.
 * @param block_array The input Block_Array from chroma processing.
 * @return A pointer to the DCT_Array containing DCT coefficients.
 */
DCT_Array *perform_dct(Block_Array *block_array);

/**
 * Performs the Inverse Discrete Cosine Transform on the DCT_Array.
 * @param dct_array The input DCT_Array with DCT coefficients.
 * @return A pointer to the reconstructed Block_Array.
 */
Block_Array *perform_idct(DCT_Array *dct_array);

/**
 * Frees the memory allocated for the DCT_Array.
 * @param dct_array The DCT_Array to be freed.
 */
void free_dct_array(DCT_Array *dct_array);

#endif /* TRANSFORM_H */
