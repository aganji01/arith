/* chroma_processing.c */

#include "chroma_processing.h"
#include <stdlib.h>
#include <assert.h>

/* Creates an array of blocks from the YPbPr image */
Block_Array *create_blocks(YPbPr_image *ypbpr_image)
{
    assert(ypbpr_image != NULL);

    int image_width = ypbpr_image->width;
    int image_height = ypbpr_image->height;

    /* Calculate the number of blocks horizontally and vertically */
    int block_width = image_width / 2;
    int block_height = image_height / 2;

    /* Allocate memory for Block_Array */
    Block_Array *block_array = malloc(sizeof(Block_Array));
    assert(block_array != NULL);

    block_array->width = block_width;
    block_array->height = block_height;

    block_array->blocks = malloc(block_height * sizeof(Block *));
    assert(block_array->blocks != NULL);

    for (int i = 0; i < block_height; i++) {
        block_array->blocks[i] = malloc(block_width * sizeof(Block));
        assert(block_array->blocks[i] != NULL);
    }

    /* Process each 2x2 block */
    for (int block_y = 0; block_y < block_height; block_y++) {
        for (int block_x = 0; block_x < block_width; block_x++) {
            /* Get the coordinates of the top-left pixel in the block */
            int x = block_x * 2;
            int y = block_y * 2;

            /* Retrieve the four pixels */
            YPbPr_pixel p1 = ypbpr_image->pixels[y][x];
            YPbPr_pixel p2 = ypbpr_image->pixels[y][x + 1];
            YPbPr_pixel p3 = ypbpr_image->pixels[y + 1][x];
            YPbPr_pixel p4 = ypbpr_image->pixels[y + 1][x + 1];

            /* Compute the average Pb and Pr values */
            float pb_avg = (p1.pb + p2.pb + p3.pb + p4.pb) / 4.0;
            float pr_avg = (p1.pr + p2.pr + p3.pr + p4.pr) / 4.0;

            /* Store the Y values and averaged chroma values in the block */
            Block *block = &block_array->blocks[block_y][block_x];
            block->y1 = p1.y;
            block->y2 = p2.y;
            block->y3 = p3.y;
            block->y4 = p4.y;
            block->pb_avg = pb_avg;
            block->pr_avg = pr_avg;
        }
    }

    return block_array;
}

/* Reassembles the YPbPr image from the array of blocks */
YPbPr_image *reassemble_blocks(Block_Array *block_array)
{
    assert(block_array != NULL);

    int block_width = block_array->width;
    int block_height = block_array->height;

    int image_width = block_width * 2;
    int image_height = block_height * 2;

    /* Allocate memory for YPbPr_image */
    YPbPr_image *ypbpr_image = malloc(sizeof(YPbPr_image));
    assert(ypbpr_image != NULL);

    ypbpr_image->width = image_width;
    ypbpr_image->height = image_height;

    ypbpr_image->pixels = malloc(image_height * sizeof(YPbPr_pixel *));
    assert(ypbpr_image->pixels != NULL);

    for (int i = 0; i < image_height; i++) {
        ypbpr_image->pixels[i] = malloc(image_width * sizeof(YPbPr_pixel));
        assert(ypbpr_image->pixels[i] != NULL);
    }

    /* Process each block to reconstruct the image */
    for (int block_y = 0; block_y < block_height; block_y++) {
        for (int block_x = 0; block_x < block_width; block_x++) {
            /* Get the coordinates of the top-left pixel in the block */
            int x = block_x * 2;
            int y = block_y * 2;

            Block *block = &block_array->blocks[block_y][block_x];

            /* Retrieve the averaged Pb and Pr values */
            float pb_avg = block->pb_avg;
            float pr_avg = block->pr_avg;

            /* Reconstruct the four pixels */
            ypbpr_image->pixels[y][x].y = block->y1;
            ypbpr_image->pixels[y][x].pb = pb_avg;
            ypbpr_image->pixels[y][x].pr = pr_avg;

            ypbpr_image->pixels[y][x + 1].y = block->y2;
            ypbpr_image->pixels[y][x + 1].pb = pb_avg;
            ypbpr_image->pixels[y][x + 1].pr = pr_avg;

            ypbpr_image->pixels[y + 1][x].y = block->y3;
            ypbpr_image->pixels[y + 1][x].pb = pb_avg;
            ypbpr_image->pixels[y + 1][x].pr = pr_avg;

            ypbpr_image->pixels[y + 1][x + 1].y = block->y4;
            ypbpr_image->pixels[y + 1][x + 1].pb = pb_avg;
            ypbpr_image->pixels[y + 1][x + 1].pr = pr_avg;
        }
    }

    return ypbpr_image;
}

/* Frees the memory allocated for the Block_Array */
void free_block_array(Block_Array *block_array)
{
    if (block_array == NULL) {
        return;
    }

    for (int i = 0; i < block_array->height; i++) {
        free(block_array->blocks[i]);
    }
    free(block_array->blocks);
    free(block_array);
}