#include "../include/bitmap.h"

int create_bitmap(uint8_t ** bitmap, uint64_t * bitmap_size) {
    (* bitmap) = (uint8_t *)calloc(sizeof(uint8_t), (* bitmap_size) / 8);
    
    if(!(* bitmap))
        return -ENOMEM;
    
    (* bitmap_size) = (* bitmap_size) / 8;

    return 0;
}

int clear_bit(uint8_t ** bitmap, uint64_t bitno) {
    uint64_t index = bitno / 8;
    int bit_index = bitno % 8;

    int val = (int)pow(2, bit_index);
    (* bitmap)[index] = (* bitmap)[index] & ~(val);

    return 0;
}

int set_bit(uint8_t ** bitmap, uint64_t bitno) {
    uint64_t index = bitno / 8;
    int bit_index = bitno % 8;

    int val = (int)pow(2, bit_index);
    (* bitmap)[index] = (* bitmap)[index] | (val);

    return 0;
}

uint64_t get_first_unset_bit(uint8_t * bitmap, uint64_t bitmap_size) {
    uint64_t index = 0;
    int bit_index = 0;
    int val, found = 0;

    while(index < bitmap_size) {
        bit_index = 0;
        while(bit_index < 8) {
            val = (int)pow(2, bit_index);
            
            if( !(bitmap[index] & val) ) {
                found = 1;
                break;
            }
            bit_index++;
        }

        if(!found)
            index++;
        else
            break;
    }

    if(found)
        return (index * 8 + bit_index);
    else
        return -1;
}

uint64_t get_first_set_bit(uint8_t * bitmap, uint64_t bitmap_size) {
    uint64_t index = 0;
    int bit_index = 0;
    int val, found = 0;

    while(index < bitmap_size) {
        bit_index = 0;
        while(bit_index < 8) {
            val = (int)pow(2, bit_index);
            
            if(bitmap[index] & val) {
                found = 1;
                break;
            }
            bit_index++;
        }

        if(!found)
            index++;
        else
            break;
    }

    if(found)
        return (index * 8 + bit_index);
    else
        return -1;
}

void print_bitmap(uint8_t * bitmap, uint64_t bitmap_size){
    printf("Little Endian\n");
    int index = 0, bit_index = 0;
    for(index = 0; index < bitmap_size; index++){
        for (bit_index = 0; bit_index < 8; bit_index++) {
            printf("%d", !!((bitmap[index] << bit_index) & 0x80));
        }
        printf(" ");
    }
    printf("\n");
}


int free_bitmap(uint8_t ** bitmap){
    free(*bitmap);
    *bitmap = NULL;
    return 0;
}
