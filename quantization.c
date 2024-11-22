/* quantization.c */

#include "quantization.h"
#include "bitpack.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>

/* Quantization constants */
#define A_SCALE_FACTOR 511.0    // For 'a' coefficient (9 bits unsigned)
#define BCD_SCALE_FACTOR 50.0   // For 'b', 'c', 'd' coefficients (5 bits signed)

/* Bit widths for packing */
#define A_WIDTH 9
#define B_WIDTH 5
#define C_WIDTH 5
#define D_WIDTH 5
#define PB_INDEX_WIDTH 4
#define PR_INDEX_WIDTH 4

/* Bit positions for packing */
#define A_LSB 23
#define B_LSB 18
#define C_LSB 13
#define D_LSB 8
#define PB_LSB 4
#define PR_LSB 0

/* Quantization and dequantization functions */
static unsigned quantize_a(float a);
static int quantize_bcd(float coefficient);
static float dequantize_a(unsigned a_quant);
static float dequantize_bcd(int bcd_quant);

/* Chroma index mapping */
static unsigned index_of_chroma(float chroma);
static float chroma_of_index(unsigned index);

/* Constants for chroma index mapping */
#define CHROMA_MIN -0.3
#define CHROMA_MAX 0.3
#define CHROMA_STEPS 15

/* Quantizes and packs DCT coefficients into codewords */
Codeword_Array *quantize_and_pack(DCT_Array *dct_array)
{
    assert(dct_array != NULL);

    int width = dct_array->width;
    int height = dct_array->height;
    int num_codewords = width * height;

    /* Allocate memory for Codeword_Array */
    Codeword_Array *codeword_array = malloc(sizeof(Codeword_Array));
    assert(codeword_array != NULL);

    codeword_array->count = num_codewords;
    codeword_array->words = malloc(num_codewords * sizeof(uint32_t));
    assert(codeword_array->words != NULL);

    int index = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            DCT_Block *dct_block = &dct_array->blocks[y][x];

            /* Quantize coefficients */
            unsigned a_quant = quantize_a(dct_block->a);
            int b_quant = quantize_bcd(dct_block->b);
            int c_quant = quantize_bcd(dct_block->c);
            int d_quant = quantize_bcd(dct_block->d);

            /* Quantize chroma values */
            unsigned pb_index = index_of_chroma(dct_block->pb_avg);
            unsigned pr_index = index_of_chroma(dct_block->pr_avg);

            /* Pack into a 32-bit codeword using bitpack functions */
            uint32_t codeword = 0;
            codeword = Bitpack_newu(codeword, A_WIDTH, A_LSB, a_quant);
            codeword = Bitpack_news(codeword, B_WIDTH, B_LSB, b_quant);
            codeword = Bitpack_news(codeword, C_WIDTH, C_LSB, c_quant);
            codeword = Bitpack_news(codeword, D_WIDTH, D_LSB, d_quant);
            codeword = Bitpack_newu(codeword, PB_INDEX_WIDTH, PB_LSB, pb_index);
            codeword = Bitpack_newu(codeword, PR_INDEX_WIDTH, PR_LSB, pr_index);

            /* Store the codeword */
            codeword_array->words[index++] = codeword;
        }
    }

    return codeword_array;
}

/* Unpacks codewords and dequantizes DCT coefficients */
DCT_Array *unpack_and_dequantize(Codeword_Array *codeword_array)
{
    assert(codeword_array != NULL);

    int num_codewords = codeword_array->count;

    /* Determine dimensions */
    int width = sqrt(num_codewords);   // Assuming square image for simplicity
    int height = width;

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

    int index = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint32_t codeword = codeword_array->words[index++];

            /* Unpack the codeword */
            unsigned a_quant = Bitpack_getu(codeword, A_WIDTH, A_LSB);
            int b_quant = Bitpack_gets(codeword, B_WIDTH, B_LSB);
            int c_quant = Bitpack_gets(codeword, C_WIDTH, C_LSB);
            int d_quant = Bitpack_gets(codeword, D_WIDTH, D_LSB);
            unsigned pb_index = Bitpack_getu(codeword, PB_INDEX_WIDTH, PB_LSB);
            unsigned pr_index = Bitpack_getu(codeword, PR_INDEX_WIDTH, PR_LSB);

            /* Dequantize coefficients */
            float a = dequantize_a(a_quant);
            float b = dequantize_bcd(b_quant);
            float c = dequantize_bcd(c_quant);
            float d = dequantize_bcd(d_quant);

            /* Retrieve chroma values */
            float pb_avg = chroma_of_index(pb_index);
            float pr_avg = chroma_of_index(pr_index);

            /* Store in DCT_Block */
            DCT_Block *dct_block = &dct_array->blocks[y][x];
            dct_block->a = a;
            dct_block->b = b;
            dct_block->c = c;
            dct_block->d = d;
            dct_block->pb_avg = pb_avg;
            dct_block->pr_avg = pr_avg;
        }
    }

    return dct_array;
}

/* Frees the memory allocated for the Codeword_Array */
void free_codeword_array(Codeword_Array *codeword_array)
{
    if (codeword_array == NULL) {
        return;
    }

    free(codeword_array->words);
    free(codeword_array);
}

/* Helper function implementations */

/* Quantizes coefficient 'a' to an unsigned integer */
static unsigned quantize_a(float a)
{
    /* Ensure 'a' is within [0,1] */
    if (a < 0.0) a = 0.0;
    if (a > 1.0) a = 1.0;

    unsigned a_quant = (unsigned)round(a * A_SCALE_FACTOR);
    return a_quant;
}

/* Dequantizes coefficient 'a' from an unsigned integer */
static float dequantize_a(unsigned a_quant)
{
    float a = (float)a_quant / A_SCALE_FACTOR;
    return a;
}

/* Quantizes coefficients 'b', 'c', 'd' to signed integers */
static int quantize_bcd(float coefficient)
{
    /* Clamp coefficient to the range [-0.3, 0.3] */
    if (coefficient < -0.3) coefficient = -0.3;
    if (coefficient > 0.3) coefficient = 0.3;

    int quantized = (int)round(coefficient * BCD_SCALE_FACTOR);
    return quantized;
}

/* Dequantizes coefficients 'b', 'c', 'd' from signed integers */
static float dequantize_bcd(int bcd_quant)
{
    float coefficient = (float)bcd_quant / BCD_SCALE_FACTOR;
    return coefficient;
}

/* Maps chroma values to an index */
static unsigned index_of_chroma(float chroma)
{
    /* Clamp chroma to the range [-0.3, 0.3] */
    if (chroma < CHROMA_MIN) chroma = CHROMA_MIN;
    if (chroma > CHROMA_MAX) chroma = CHROMA_MAX;

    unsigned index = (unsigned)round(((chroma - CHROMA_MIN) / (CHROMA_MAX - CHROMA_MIN)) * CHROMA_STEPS);
    if (index > CHROMA_STEPS) index = CHROMA_STEPS;
    return index;
}

/* Retrieves chroma value from an index */
static float chroma_of_index(unsigned index)
{
    if (index > CHROMA_STEPS) index = CHROMA_STEPS;
    float chroma = CHROMA_MIN + ((float)index / CHROMA_STEPS) * (CHROMA_MAX - CHROMA_MIN);
    return chroma;
}