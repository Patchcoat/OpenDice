#include <stdio.h>
#include <string.h>
#include "arena.h"

void memory_failure() {
    printf("ERROR: Cannot allocate enough memory\n");
    exit(1);
}

Arena new_arena(size_t size) {
    unsigned char* new_bytes = (unsigned char*) malloc(size);
    if (new_bytes == NULL) {
        memory_failure();
    }
    Arena new_arena = {
        .bytes = new_bytes,
        .next = new_bytes,
        .end = new_bytes + size,
    };
    return new_arena;
}

void grow_arena(Arena *arena, size_t new_size) {
    unsigned char* new_bytes = (unsigned char*) realloc(arena->bytes, new_size);
    if (new_bytes == NULL) {
        memory_failure();
    }
    arena->bytes = new_bytes;
    arena->end = new_bytes + new_size;
}

unsigned char* push_to_arena(Arena *arena, size_t size) {
    if ((arena->next + size) > arena->end) {
        size_t new_arena_size = arena->end - arena->bytes;
        new_arena_size += size;
        new_arena_size *= 1.5;
        grow_arena(arena, new_arena_size);
    }
    unsigned char *next = arena->next;
    arena->next += size;
    return next;
}

void add_string(Arena *arena, char* string) {
    size_t size = strlen(string);
    if ((arena->next + size) > arena->end) {
        size_t new_arena_size = arena->end - arena->bytes;
        new_arena_size += size;
        new_arena_size *= 1.5;
        grow_arena(arena, new_arena_size);
    }
    unsigned char *next = arena->next;
    arena->next += size;
    strcpy_s((char*) next, size, string);
}

void free_arena(Arena *arena) {
    free(arena->bytes);
    arena->bytes = NULL;
    arena->next = NULL;
    arena->end = NULL;
}