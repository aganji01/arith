/* uarray2.h */

#ifndef UARRAY2_H
#define UARRAY2_H

#include <stdlib.h>

typedef struct UArray2_T *UArray2_T;

/* Creates a new 2D array */
UArray2_T UArray2_new(int width, int height, size_t size);

/* Returns a pointer to the element at (x, y) */
void *UArray2_at(UArray2_T array2, int x, int y);

/* Frees the 2D array */
void UArray2_free(UArray2_T *array2);

#endif /* UARRAY2_H */
