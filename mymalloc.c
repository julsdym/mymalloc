#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "mymalloc.h"

#define MEMLENGTH 4096
static union {
    char bytes[MEMLENGTH];
    double not_used; 
} heap;

typedef struct header {
    size_t size;       
    int is_free;        // 1 = free, 0 = allocated
} header;

// Align any size up to multiple of 8
static inline size_t align8(size_t n) {
    return (n + 7) & ~7;
}

// Global heap state
static header *heap_start = NULL;
static int heap_initialized = 0;

// Get next header in heap (returns NULL if at end)
static header *get_next_header(header *curr) {
    char *next_addr = (char *)curr + curr->size;
    char *heap_end = heap.bytes + MEMLENGTH;
    
    if (next_addr >= heap_end) {
        return NULL; 
    }
    return (header *)next_addr;
}

// Get first header in heap
static header *get_first_header(void) {
    return heap_start;
}

// Leak checker
void check_leaks(void) {
    if (!heap_initialized) return;

    size_t leaked_bytes = 0;
    size_t leaked_objs = 0;
    
    for (header *curr = get_first_header(); curr; curr = get_next_header(curr)) {
        if (!curr->is_free) {
            leaked_bytes += curr->size - sizeof(header);
            leaked_objs++;
        }
    }
    
    if (leaked_objs > 0) {
        fprintf(stderr,
                "mymalloc: %zu bytes leaked in %zu objects.\n",
                leaked_bytes, leaked_objs);
    }
}

// Initialize heap
static void init_heap(void) {
    if (heap_initialized) return;
    heap_start = (header *)heap.bytes;
    heap_start->size = MEMLENGTH; 
    heap_start->is_free = 1;
    heap_initialized = 1;
    atexit(check_leaks);
}

// Validate pointer, return header if valid
static header *get_valid_header(void *ptr) {
    if (!ptr) return NULL;

    for (header *curr = get_first_header(); curr; curr = get_next_header(curr)) {
        if ((char *)curr + sizeof(header) == (char *)ptr) {
            return curr; 
        }
    }
    return NULL; 
}

// Split a free chunk if it's large enough
static void split_chunk(header *chunk, size_t req_size) {
    size_t total_req = sizeof(header) + align8(req_size);
    size_t min_remaining = sizeof(header) + 8; // Minimum size for a new block

    if (chunk->size >= total_req + min_remaining) {
        header *new_header = (header *)((char *)chunk + total_req);
        new_header->size = chunk->size - total_req;
        new_header->is_free = 1;
        chunk->size = total_req;
    }
}

// Coalescing Function
static void coalesce(void) {
    for (header *curr = get_first_header(); curr; curr = get_next_header(curr)) {
        if (curr->is_free) {
            header *next = get_next_header(curr);
            while (next && next->is_free) {
                curr->size += next->size;
                next = get_next_header(curr); 
            }
        }
    }
}

void *mymalloc(size_t size, char *file, int line) {
    if (!heap_initialized) init_heap();
    if (size == 0) return NULL;

    size = align8(size);
    size_t total_req = sizeof(header) + size;

    for (header *curr = get_first_header(); curr; curr = get_next_header(curr)) {
        if (curr->is_free && curr->size >= total_req) {
            split_chunk(curr, size);
            curr->is_free = 0;
            return (char *)curr + sizeof(header);
        }
    }

    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n",
            size, file, line);
    return NULL;
}

void myfree(void *ptr, char *file, int line) {
    if (ptr == NULL) return;

    if ((char *)ptr < heap.bytes || (char *)ptr >= heap.bytes + MEMLENGTH) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    header *h = get_valid_header(ptr);

    if (!h) {
        fprintf(stderr,
                "free: Inappropriate pointer. Not at start of chunk. (%s:%d)\n",
                file, line);
        exit(2);
    }

    if (h->is_free) {
        fprintf(stderr,
                "free: Inappropriate pointer. Double free detected. (%s:%d)\n",
                file, line);
        exit(2);
    }

    h->is_free = 1;
    coalesce();
}
