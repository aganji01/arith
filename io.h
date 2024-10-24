/* io.h */

#ifndef IO_H
#define IO_H

#include "quantization.h"  // For Codeword_Array
#include "image_processing.h"  // For Image

/**
 * Writes the compressed image data to the output file.
 * @param output The output file pointer.
 * @param codeword_array The Codeword_Array containing codewords.
 * @param width The width of the original image.
 * @param height The height of the original image.
 */
void write_compressed_image(FILE *output, Codeword_Array *codeword_array, int width, int height);

/**
 * Writes the Image data to the output file in PPM format.
 * @param output The output file pointer.
 * @param image The Image to be written.
 */
void write_image(FILE *output, Image *image);

#endif /* IO_H */