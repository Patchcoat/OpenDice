#ifndef _BCD_H
#define _BCD_H
#include <stdlib.h>
#include "arena.h"

#define ISDIGIT(x) (x >= 0x30 && x <= 0x39)
#define ISCHAR(x) ((x >= 0x41 && x <= 0x5A) || (x >= 0x61 && x <= 0x7A))

typedef union {
    struct {
        char first:4;
        char second:4;
    };
    char both;
} DigitPair;

typedef struct {
    size_t length;
    size_t decimal_place;
    bool decimal;
    bool negative;
    DigitPair* digits;
} Num;

Num *new_num(Arena *arena);
void add_digit(Arena *arena, Num *num, char digit_in);
int get_digit(Num *num, size_t digit);


#endif