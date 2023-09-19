#ifndef _OPERATOR_H
#define _OPERATOR_H
#include <stdlib.h>
#include <stdbool.h>
#include "arena.h"

typedef struct {
    char* string;
    size_t length;
    bool is_biterm;
} Operator;

Operator* new_operator(Arena *arena);
void add_symbol(Arena *arena, Operator* opt, char symbol_in);

#endif