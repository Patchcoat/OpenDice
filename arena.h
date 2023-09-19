#ifndef _ARENA_H
#define _ARENA_H
#include <stdlib.h>

typedef struct {
    unsigned char *bytes;
    unsigned char *next;
    unsigned char *end;
} Arena;

Arena new_arena(size_t size);
void grow_arena(Arena *arena, size_t new_size);
unsigned char* push_to_arena(Arena *arena, size_t size);
void add_string(Arena *arena, char* string);
void free_arena(Arena *arena);

#endif