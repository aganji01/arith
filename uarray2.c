/* uarray2.c */

#include "uarray2.h"
#include <assert.h>

struct UArray2_T {
    int width;
    int height;
    size_t size;
    void *data;
};

UArray2_T UArray2_new(int width, int height, size_t size)
{
    assert(width > 0 && height > 0 && size > 0);
    UArray2_T array2 = malloc(sizeof(*array2));
    assert(array2 != NULL);
    array2->width = width;
    array2->height = height;
    array2->size = size;
    array2->data = calloc(width * height, size);
    assert(array2->data != NULL);
    return array2;
}

void *UArray2_at(UArray2_T array2, int x, int y)
{
    assert(array2 != NULL);
    assert(x >= 0 && x < array2->width);
    assert(y >= 0 && y < array2->height);
    char *data = array2->data;
    return data + ((y * array2->width + x) * array2->size);
}

void UArray2_free(UArray2_T *array2)
{
    assert(array2 != NULL && *array2 != NULL);
    free((*array2)->data);
    free(*array2);
    *array2 = NULL;
}
