#ifndef BITMAP_H
#define BITMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <errno.h>

/*
Create a bitmap with bitmap_size bits. The bitmap is stored in `bitmap` and the size, in bytes, is stores in `bitmap_size`, however, the function call will contain bits.
*/
int create_bitmap(uint8_t ** bitmap, uint64_t * bitmap_size);

/*
Print the bitmap to stdout.
*/
void print_bitmap(uint8_t * bitmap, uint64_t bitmap_size);

/*
Free the heap memory taken up by the bitmap.
*/
int free_bitmap(uint8_t ** bitmap);

/*
Clear the bit at `bitno` of bitmap, i.e, set it to 0. If `bitno` is greated than size of the bitmap, undefined behaviour occurs.
*/
int clear_bit(uint8_t ** bitmap, uint64_t bitno);

/*
Set the bit at `bitno` of bitmap, i.e, set it to 1. If `bitno` is greated than size of the bitmap, undefined behaviour occurs.
*/
int set_bit(uint8_t ** bitmap, uint64_t bitno);

/*
Get the first bit of `bitmap` that is set to 1.
*/
uint64_t get_first_set_bit(uint8_t * bitmap, uint64_t bitmap_size);

/*
Get the first bit of `bitmap` that is unset, i.e, set to 0.
*/
uint64_t get_first_unset_bit(uint8_t * bitmap, uint64_t bitmap_size); 

#endif