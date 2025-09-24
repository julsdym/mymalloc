#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "mymalloc.h"

#define MEMLENGTH 4096
static union {
    char bytes[MEMLENGTH];
    double not_used; // ensures 8-byte alignment
} heap;

typedef struct header {
    size_t size;
    int is_free; // 1 = free, 0 = allocated
} header;

// Align size up to multiple of 8
static inline size_t align8(size_t n) {
    return (n + 7) & ~7;
}

// Global heap state
static int heap_initialized = 0;

// Leak checker
void check_leaks(void) {
    if (!heap_initialized) return;

    size_t leaked_bytes = 0;
    size_t leaked_objs = 0;

    char *curr_ptr = heap.bytes;
    while (curr_ptr < heap.bytes + MEMLENGTH) {
        header *curr = (header *)curr_ptr;
        if (curr->size == 0) break; // end of initialized heap
        if (!curr->is_free) {
            leaked_bytes += curr->size;
            leaked_objs++;
        }
        curr_ptr += sizeof(header) + curr->size;
    }

    if (leaked_objs > 0) {
        fprintf(stderr, "mymalloc: %zu bytes leaked in %zu objects.\n",
                leaked_bytes, leaked_objs);
    }
}

// Initialize heap
static void init_heap(void) {
    if (heap_initialized) return;
    header *first = (header *)heap.bytes;
    first->size = MEMLENGTH - sizeof(header);
    first->is_free = 1;
    heap_initialized = 1;
    atexit(check_leaks);
}

// Helper: find a valid header for a pointer
static header *get_valid_header(void *ptr) {
    if (!ptr) return NULL;

    char *curr_ptr = heap.bytes;
    while (curr_ptr < heap.bytes + MEMLENGTH) {
        header *curr = (header *)curr_ptr;
        if (curr->size == 0) break;
        if ((char *)curr + sizeof(header) == (char *)ptr)
            return curr;
        curr_ptr += sizeof(header) + curr->size;
    }
    return NULL;
}

// Split a free chunk if large enough
static void split_chunk(header *chunk, size_t req_size) {
    if (chunk->size >= req_size + sizeof(header) + 8) {
        header *new_chunk = (header *)((char *)chunk + sizeof(header) + req_size);
        new_chunk->size = chunk->size - req_size - sizeof(header);
        new_chunk->is_free = 1;
        chunk->size = req_size;
    }
}

// Find next chunk in memory
static header *next_chunk(header *curr) {
    char *next = (char *)curr + sizeof(header) + curr->size;
    if (next >= heap.bytes + MEMLENGTH) return NULL;
    header *n = (header *)next;
    if ((char *)n + sizeof(header) > heap.bytes + MEMLENGTH) return NULL;
    return n;
}

// Coalesce adjacent free chunks
static void coalesce(void) {
    char *curr_ptr = heap.bytes;
    while (curr_ptr < heap.bytes + MEMLENGTH) {
        header *curr = (header *)curr_ptr;
        if (curr->size == 0) break;
        header *next = next_chunk(curr);
        if (!next) break;
        if (curr->is_free && next->is_free) {
            curr->size += sizeof(header) + next->size;
            continue; // check new next
        }
        curr_ptr += sizeof(header) + curr->size;
    }
}

// malloc implementation
void *mymalloc(size_t size, char *file, int line) {
    if (!heap_initialized) init_heap();
    if (size == 0) return NULL;

    size = align8(size);

    char *curr_ptr = heap.bytes;
    while (curr_ptr < heap.bytes + MEMLENGTH) {
        header *curr = (header *)curr_ptr;
        if (curr->size == 0) break;
        if (curr->is_free && curr->size >= size) {
            split_chunk(curr, size);
            curr->is_free = 0;
            return (char *)curr + sizeof(header);
        }
        curr_ptr += sizeof(header) + curr->size;
    }

    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
    return NULL;
}

// free implementation
void myfree(void *ptr, char *file, int line) {
    if (!ptr) return;

    if ((char *)ptr < heap.bytes || (char *)ptr >= heap.bytes + MEMLENGTH) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    header *h = get_valid_header(ptr);
    if (!h) {
        fprintf(stderr, "free: Inappropriate pointer. Not at start of chunk (%s:%d)\n",
                file, line);
        exit(2);
    }

    if (h->is_free) {
        fprintf(stderr, "free: Inappropriate pointer. Double free detected (%s:%d)\n",
                file, line);
        exit(2);
    }

    h->is_free = 1;
    coalesce();
}
