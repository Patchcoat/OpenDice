#include "operator.h"

Operator* new_operator(Arena *arena) {
    Operator* opt = (Operator*) push_to_arena(arena, sizeof(Operator));
    opt->length = 0;
    opt->string = (char*) push_to_arena(arena, sizeof(char));
    opt->is_biterm = true;
    return opt;
}

void add_symbol(Arena *arena, Operator* opt, char symbol_in) {
    if ((arena->next - (unsigned char*) opt->string) <= opt->length) {
        push_to_arena(arena, sizeof(char));
    }
    opt->string[opt->length] = symbol_in;
    opt->length++;
}