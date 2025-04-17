#ifndef ARENA_H
#define ARENA_H

typedef struct Arena {
    void* memory;
    void* offset_ptr;
    int size;
} Arena;

Arena* arena_init();

void* arena_alloc(Arena* arena, int size);

void arena_free(Arena* arena);

#endif
