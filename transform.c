/* transform.c */

#include "transform.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>

/* Helper functions for DCT calculations */
static float calculate_a(float y1, float y2, float y3, float y4);
static float calculate_b(float y1, float y2, float y3, float y4);
static float calculate_c(float y1, float y2, float y3, float y4);
static float calculate_d(float y1, float y2, float y3, float y4);

static void calculate_y_values(float a, float b, float c, float d,
                               float *y1, float *y2, float *y3, float *y4);

/* Performs the Discrete Cosine Transform on the Block_Array */
DCT_Array *perform_dct(Block_Array *block_array)
{
    assert(block_array != NULL);

    int width = block_array->width;
    int height = block_array->height;

    /* Allocate memory for DCT_Array */
    DCT_Array *dct_array = malloc(sizeof(DCT_Array));
    assert(dct_array != NULL);

    dct_array->width = width;
    dct_array->height = height;

    dct_array->blocks = malloc(height * sizeof(DCT_Block *));
    assert(dct_array->blocks != NULL);

    for (int i = 0; i < height; i++) {
        dct_array->blocks[i] = malloc(width * sizeof(DCT_Block));
        assert(dct_array->blocks[i] != NULL);
    }

    /* Perform DCT on each block */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Block *block = &block_array->blocks[y][x];
            DCT_Block *dct_block = &dct_array->blocks[y][x];

            /* Calculate DCT coefficients */
            float a = calculate_a(block->y1, block->y2, block->y3, block->y4);
            float b = calculate_b(block->y1, block->y2, block->y3, block->y4);
            float c = calculate_c(block->y1, block->y2, block->y3, block->y4);
            float d = calculate_d(block->y1, block->y2, block->y3, block->y4);

            /* Store the coefficients and chroma values */
            dct_block->a = a;
            dct_block->b = b;
            dct_block->c = c;
            dct_block->d = d;
            dct_block->pb_avg = block->pb_avg;
            dct_block->pr_avg = block->pr_avg;
        }
    }

    return dct_array;
}

/* Performs the Inverse Discrete Cosine Transform on the DCT_Array */
Block_Array *perform_idct(DCT_Array *dct_array)
{
    assert(dct_array != NULL);

    int width = dct_array->width;
    int height = dct_array->height;

    /* Allocate memory for Block_Array */
    Block_Array *block_array = malloc(sizeof(Block_Array));
    assert(block_array != NULL);

    block_array->width = width;
    block_array->height = height;

    block_array->blocks = malloc(height * sizeof(Block *));
    assert(block_array->blocks != NULL);

    for (int i = 0; i < height; i++) {
        block_array->blocks[i] = malloc(width * sizeof(Block));
        assert(block_array->blocks[i] != NULL);
    }

    /* Perform IDCT on each block */
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            DCT_Block *dct_block = &dct_array->blocks[y][x];
            Block *block = &block_array->blocks[y][x];

            /* Calculate Y values from DCT coefficients */
            float y1, y2, y3, y4;
            calculate_y_values(dct_block->a, dct_block->b, dct_block->c, dct_block->d,
                               &y1, &y2, &y3, &y4);

            /* Store the Y values and chroma values */
            block->y1 = y1;
            block->y2 = y2;
            block->y3 = y3;
            block->y4 = y4;
            block->pb_avg = dct_block->pb_avg;
            block->pr_avg = dct_block->pr_avg;
        }
    }

    return block_array;
}

/* Frees the memory allocated for the DCT_Array */
void free_dct_array(DCT_Array *dct_array)
{
    if (dct_array == NULL) {
        return;
    }

    for (int i = 0; i < dct_array->height; i++) {
        free(dct_array->blocks[i]);
    }
    free(dct_array->blocks);
    free(dct_array);
}

/* Helper function implementations */

/* Calculates coefficient a */
static float calculate_a(float y1, float y2, float y3, float y4)
{
    return (y4 + y3 + y2 + y1) / 4.0;
}

/* Calculates coefficient b */
static float calculate_b(float y1, float y2, float y3, float y4)
{
    return (y4 + y3 - y2 - y1) / 4.0;
}

/* Calculates coefficient c */
static float calculate_c(float y1, float y2, float y3, float y4)
{
    return (y4 - y3 + y2 - y1) / 4.0;
}

/* Calculates coefficient d */
static float calculate_d(float y1, float y2, float y3, float y4)
{
    return (y4 - y3 - y2 + y1) / 4.0;
}

/* Calculates Y values from DCT coefficients */
static void calculate_y_values(float a, float b, float c, float d,
                               float *y1, float *y2, float *y3, float *y4)
{
    *y1 = a - b - c + d;
    *y2 = a - b + c - d;
    *y3 = a + b - c - d;
    *y4 = a + b + c + d;
}