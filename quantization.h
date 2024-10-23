/* quantization.h */

#ifndef QUANTIZATION_H
#define QUANTIZATION_H

#include "transform.h"  // For DCT_Array and DCT_Block
#include <stdint.h>

/* Structure to represent an array of codewords */
typedef struct {
    int count;       // Number of codewords
    uint32_t *words; // Array of 32-bit codewords
} Codeword_Array;

/* Function Prototypes */

/**
 * Quantizes the DCT coefficients and packs them into 32-bit codewords.
 * @param dct_array The input DCT_Array containing DCT coefficients.
 * @return A pointer to the Codeword_Array containing packed codewords.
 */
Codeword_Array *quantize_and_pack(DCT_Array *dct_array);

/**
 * Unpacks the 32-bit codewords and dequantizes the DCT coefficients.
 * @param codeword_array The input Codeword_Array containing codewords.
 * @return A pointer to the reconstructed DCT_Array with dequantized coefficients.
 */
DCT_Array *unpack_and_dequantize(Codeword_Array *codeword_array);

/**
 * Frees the memory allocated for the Codeword_Array.
 * @param codeword_array The Codeword_Array to be freed.
 */
void free_codeword_array(Codeword_Array *codeword_array);

#endif /* QUANTIZATION_H */
