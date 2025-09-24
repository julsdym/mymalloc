#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "mymalloc.h"

#define MEMLENGTH 4096
static union {
    char bytes[MEMLENGTH];
    double align;   // ensures max alignment
} heap;

typedef struct header {
    size_t size;
    int is_free;            // 1 = free, 0 = allocated
    struct header *next;
} header;


// Align any size up to multiple of 8
static inline size_t align8(size_t n) {
    return (n + 7) & ~7;
}

// Global heap state
static header *heap_start = NULL;
static int heap_initialized = 0;

// Leak checker
void check_leaks(void) {
    if (!heap_initialized) return;

    size_t leaked_bytes = 0;
    size_t leaked_objs = 0;
    for (header *curr = heap_start; curr; curr = curr->next) {
        if (!curr->is_free) {
            leaked_bytes += curr->size;
            leaked_objs++;
        }
    }
    if (leaked_objs > 0) {
        fprintf(stderr, "mymalloc: %zu bytes leaked in %zu objects.\n", leaked_bytes, leaked_objs);
    }
}

// Initialize heap
static void init_heap(void) {
    if (heap_initialized) return;
    heap_start = (header *)heap.bytes;
    heap_start->size = MEMLENGTH - sizeof(header);
    heap_start->is_free = 1;
    heap_start->next = NULL;
    heap_initialized = 1;
    atexit(check_leaks);
}

//HELPER FUNCTIONS

//is the pointer at the start of a chunk/valid?
static header *get_valid_header(void *ptr) {
    if (!ptr) return NULL;

    for (header *curr = heap_start; curr; curr = curr->next) {
        if ((char *)curr + sizeof(header) == (char *)ptr) {
            return curr; // return header directly
        }
    }
    return NULL; // not valid
}

// Split a free chunk if it’s large enough
static void split_chunk(header *chunk, size_t req_size) {
    if (chunk->size >= req_size + sizeof(header) + 8) {
        header *new = (header *)((char *)chunk + sizeof(header) + req_size);
        new->size = chunk->size - req_size - sizeof(header);
        new->is_free = 1;
        new->next = chunk->next;
        chunk->size = req_size;
        chunk->next = new;
    }
}

// Merge adjacent free chunks
static void coalesce(void) {
    header *curr = heap_start;
    while (curr && curr->next) {
        if (curr->is_free && curr->next->is_free) {
            curr->size += sizeof(header) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

// malloc implementation
void *mymalloc(size_t size, char *file, int line) {
    if (!heap_initialized) init_heap();
    if (size == 0) return NULL;

    //size of payload
    size = align8(size);

    //find suitable chunk, split it
    for (header *curr = heap_start; curr; curr = curr->next) {
        if (curr->is_free && curr->size >= size) {
            split_chunk(curr, size);
            //allocate memory at that chunk
            curr->is_free = 0;
            return (char *)curr + sizeof(header);
        }
    }
    //return error if cannot find chunk
    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
    return NULL;
}

// free implementation
void myfree(void *ptr, char *file, int line) {
    if (ptr==NULL) return;

    //check to see if pointer is within bounds of heap
    if ((char *)ptr < heap.bytes || (char *)ptr >= heap.bytes + MEMLENGTH) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    //check to see if header is at start of a chunk
    header *h = get_valid_header(ptr);

     //check to see if the ptr is at start of a chunk
    if(!h){
      fprintf(stderr, "free: Inappropriate pointer. Not at start of chunk. (%s:%d)\n", file, line);
      exit(2);
      }
    
    //double free detection
    if (h->is_free) {
        fprintf(stderr, "free: Innappropriate pointer. Double free detected. (%s:%d)\n", file, line);
        exit(2);
    }
    //free block + coalesce chunks
    h->is_free = 1;
    coalesce();
}

