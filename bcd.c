#include <stdbool.h>
#include <stdio.h>
#include "bcd.h"

Num *new_num(Arena *arena) {
    Num* new_num = (Num*) push_to_arena(arena, sizeof(Num));
    new_num->length = 0;
    new_num->decimal_place = 0;
    new_num->negative = false;
    new_num->decimal = false;
    new_num->digits = (DigitPair*) push_to_arena(arena, sizeof(DigitPair));
    return new_num;
}

void append_nibble(Arena *arena, Num *num, char nibble) {
    // Find the exact position to place the nibble
    size_t place = num->length / 2;
    int sub_place = num->length % 2;
    if ((arena->next - (unsigned char*) num->digits) <= num->length/2) {
        push_to_arena(arena, sizeof(char));
    }
    nibble = nibble & 0xF;
    if (sub_place == 1) {
        num->digits[place].first = nibble;
    } else {
        num->digits[place].second = nibble;
    }
    num->length++;
}

void add_digit(Arena *arena, Num *num, char digit_in) {
    if (digit_in == '.') {
        if (num->decimal) {
            printf("ERROR: Too many decimal places in number\n");
        }
        num->decimal = true;
        num->decimal_place = num->length;
        return;
    }
    if (digit_in < 0x30 && digit_in > 0x39) {
        return;
    }
    append_nibble(arena, num, digit_in);
}

int get_digit(Num *num, size_t digit) {
    if (digit >= num->length) {
        return -1;
    }
    size_t place = digit / 2;
    int sub_place = digit % 2;
    return (int) (sub_place ? num->digits[place].first : num->digits[place].second);
}