/* chroma_processing.h */

#ifndef CHROMA_PROCESSING_H
#define CHROMA_PROCESSING_H

#include "color_conversion.h"  // For YPbPr_image and YPbPr_pixel

/* Structure to represent a 2x2 block of pixels */
typedef struct {
    /* Luminance (Y) values for the 4 pixels */
    float y1;
    float y2;
    float y3;
    float y4;

    /* Averaged chrominance (Pb and Pr) values for the block */
    float pb_avg;
    float pr_avg;
} Block;

/* Structure to represent an array of blocks */
typedef struct {
    int width;   // Number of blocks horizontally
    int height;  // Number of blocks vertically
    Block **blocks;
} Block_Array;

/* Function Prototypes */

/**
 * Creates an array of blocks from the YPbPr image by averaging the
 * Pb and Pr components over each 2x2 block.
 * @param ypbpr_image The input YPbPr image.
 * @return A pointer to the Block_Array.
 */
Block_Array *create_blocks(YPbPr_image *ypbpr_image);

/**
 * Reassembles the YPbPr image from the array of blocks by applying
 * the averaged Pb and Pr values to each pixel in the 2x2 block.
 * @param block_array The input Block_Array.
 * @return A pointer to the reconstructed YPbPr_image.
 */
YPbPr_image *reassemble_blocks(Block_Array *block_array);

/**
 * Frees the memory allocated for the Block_Array.
 * @param block_array The Block_Array to be freed.
 */
void free_block_array(Block_Array *block_array);

#endif /* CHROMA_PROCESSING_H */
