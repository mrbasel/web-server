#include <stdlib.h>
#include <string.h>
#include "arena/arena.h"

Arena* arena_init(int arena_size) {
    Arena* arena = malloc(sizeof(Arena));
    arena->size = arena_size;
    arena->memory = malloc(arena_size);
    arena->offset_ptr = arena->memory;
    memset(arena->memory, 0, arena_size);
    return arena;
}

void* arena_alloc(Arena* arena, int size) {
    if ((arena->offset_ptr + size) > (arena->memory + arena->size)) return NULL;
    void* addr = arena->offset_ptr;
    arena->offset_ptr += size;
    return addr;
}

void* arena_free(Arena* arena) {
    free(arena->memory);
    free(arena);
}
