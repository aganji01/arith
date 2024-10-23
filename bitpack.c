/* bitpack.c */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bitpack.h"

/* Exception handling (define Bitpack_Overflow exception if not provided) */
Except_T Bitpack_Overflow = { "Overflow packing bits" };

/* Function to check if an unsigned value fits in the given width */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    assert(width <= 64);
    if (width == 0)
        return n == 0;
    return n < ((uint64_t)1 << width);
}

/* Function to check if a signed value fits in the given width */
bool Bitpack_fitss(int64_t n, unsigned width)
{
    assert(width <= 64);
    if (width == 0)
        return n == 0;
    int64_t min = -((int64_t)1 << (width - 1));
    int64_t max = ((int64_t)1 << (width - 1)) - 1;
    return n >= min && n <= max;
}

/* Function to extract an unsigned field from a word */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width + lsb <= 64);
    if (width == 0)
        return 0;
    uint64_t mask = (((uint64_t)1 << width) - 1) << lsb;
    return (word & mask) >> lsb;
}

/* Function to extract a signed field from a word */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width + lsb <= 64);
    if (width == 0)
        return 0;
    uint64_t ufield = Bitpack_getu(word, width, lsb);
    int64_t field = (int64_t)(ufield << (64 - width)) >> (64 - width);
    return field;
}

/* Function to insert an unsigned field into a word */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value)
{
    assert(width + lsb <= 64);
    if (!Bitpack_fitsu(value, width)) {
        RAISE(Bitpack_Overflow);
    }
    uint64_t mask = ~((((uint64_t)1 << width) - 1) << lsb);
    word = (word & mask) | (value << lsb);
    return word;
}

/* Function to insert a signed field into a word */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, int64_t value)
{
    assert(width + lsb <= 64);
    if (!Bitpack_fitss(value, width)) {
        RAISE(Bitpack_Overflow);
    }
    uint64_t mask = ~((((uint64_t)1 << width) - 1) << lsb);
    uint64_t uvalue = ((uint64_t)value) & (((uint64_t)1 << width) - 1);
    word = (word & mask) | (uvalue << lsb);
    return word;
}
