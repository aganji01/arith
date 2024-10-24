#include <string.h>
#include <assert.h>
#include <a2plain.h>
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

typedef A2Methods_UArray2 A2;   // Private abbreviation for UArray2_T

/* Create a new UArray2 */
static A2 new(int width, int height, int size)
{
    return UArray2_new(width, height, size);
}

/* Create a new UArray2 with blocksize (ignored for plain arrays) */
static A2 new_with_blocksize(int width, int height, int size, int blocksize)
{
    (void)blocksize; // Blocksize is ignored for plain arrays
    return UArray2_new(width, height, size);
}

/* Free the UArray2 */
static void a2free(A2 *array2p)
{
    UArray2_free((UArray2_T *)array2p);
}

/* Get the width of the UArray2 */
static int width(A2 array2)
{
    return UArray2_width(array2);
}

/* Get the height of the UArray2 */
static int height(A2 array2)
{
    return UArray2_height(array2);
}

/* Get the size of each element in the UArray2 */
static int size(A2 array2)
{
    return UArray2_size(array2);
}

/* Get the blocksize of the UArray2 (always 1 for plain arrays) */
static int blocksize(A2 array2)
{
    (void)array2;
    return 1; // UArray2 does not use blocks
}

/* Get a pointer to the element at position (i, j) */
static A2Methods_Object *at(A2 array2, int i, int j)
{
    return UArray2_at(array2, i, j);
}

/* Mapping functions */

/* Closure structure for apply functions */
struct closure {
    A2Methods_applyfun *apply; // Declare as a pointer to function type
    void               *cl;
};

/* Wrapper function to adapt A2Methods_applyfun to UArray2_applyfun */
static void apply_wrapper(int i, int j, UArray2_T array2,
                          void *elem, void *vcl)
{
    struct closure *mycl = vcl;
    mycl->apply(i, j, (A2)array2, elem, mycl->cl);
}

/* Map over the array in row-major order */
static void map_row_major(A2 array2, A2Methods_applyfun *apply, void *cl)
{
    struct closure mycl = { apply, cl };
    UArray2_map_row_major(array2, apply_wrapper, &mycl);
}

/* Map over the array in column-major order */
static void map_col_major(A2 array2, A2Methods_applyfun *apply, void *cl)
{
    struct closure mycl = { apply, cl };
    UArray2_map_col_major(array2, apply_wrapper, &mycl);
}

/* Small mapping functions */

/* Closure structure for small apply functions */
struct small_closure {
    A2Methods_smallapplyfun *apply; // Pointer to function type
    void                    *cl;
};

/* Wrapper function to adapt A2Methods_smallapplyfun to UArray2_applyfun */
static void apply_small_wrapper(int i, int j, UArray2_T array2,
                                void *elem, void *vcl)
{
    struct small_closure *mycl = vcl;
    (void)i;       // Unused parameter
    (void)j;       // Unused parameter
    (void)array2;  // Unused parameter
    mycl->apply(elem, mycl->cl); // Pass only elem and cl
}

/* Small map over the array in row-major order */
static void small_map_row_major(A2 array2, A2Methods_smallapplyfun *apply,
                                void *cl)
{
    struct small_closure mycl = { apply, cl };
    UArray2_map_row_major(array2, apply_small_wrapper, &mycl);
}

/* Small map over the array in column-major order */
static void small_map_col_major(A2 array2, A2Methods_smallapplyfun *apply,
                                void *cl)
{
    struct small_closure mycl = { apply, cl };
    UArray2_map_col_major(array2, apply_small_wrapper, &mycl);
}

/* Since UArray2 does not support block mapping, set these to NULL */
#define map_block_major       NULL
#define small_map_block_major NULL

/* The method suite */
static struct A2Methods_T uarray2_methods_plain_struct = {
    new,
    new_with_blocksize,
    a2free,
    width,
    height,
    size,
    blocksize,
    at,
    map_row_major,        /* map_row_major */
    map_col_major,        /* map_col_major */
    map_block_major,      /* map_block_major (NULL) */
    map_row_major,        /* map_default (row-major mapping) */
    small_map_row_major,  /* small_map_row_major */
    small_map_col_major,  /* small_map_col_major */
    small_map_block_major,/* small_map_block_major (NULL) */
    small_map_row_major   /* small_map_default (row-major mapping) */
};

/* Exported pointer to the method suite */
A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;