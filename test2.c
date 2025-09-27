#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "malloc.h"

// Helper macro for alignment check
#define IS_ALIGNED(ptr) (((uintptr_t)(ptr) % (8)) == 0)

// Basic allocation and free
void test_basic_allocation() {
    printf("Test 1: Basic Allocation/Free\n");
    void *ptr = malloc(100);
    assert(ptr != NULL);
    free(ptr);
}

// Test multiple allocations
void test_multiple_allocation(){
    printf("Test 2: Multiple Allocations\n");
    char *p2 = malloc(200);
    char *p3 = malloc(300);
    char *p4 = malloc(400);
    if (p2 && p3 && p4) {
        strcpy(p2, "aaa");
        strcpy(p3, "bbb");
        strcpy(p4, "ccc");
        printf("p2=%s, p3=%s, p4=%s\n", p2, p3, p4);
    }
    free(p2);
    free(p3);
    free(p4);

}

// Split chunks
void test_split_chunks(){
    printf("Test 3: Split Chunks\n");
    char *p5 = malloc(50);  
    char *p6 = malloc(50);
    printf("p5=%p, p6=%p\n", (void*)p5, (void*)p6);
    free(p5);
    free(p6);
}

// Alignment test
void test_alignment() {
    printf("Test 4: 8-byte Alignment\n");
    for (int i = 1; i <= 256; i *= 2) {
        void *ptr = malloc(i);
        assert(ptr != NULL);
        assert(IS_ALIGNED(ptr));
        free(ptr);
    }
}

// Coalescing test
void test_coalescing() {
    printf("Test 5: Coalescing Adjacent Blocks\n");
    void *a = malloc(100);
    void *b = malloc(200);
    void *c = malloc(150);

    free(a);
    free(c);
    free(b); 
}

// Zero-byte allocation
void test_zero_allocation() {
    printf("Test 6: Zero-byte Allocation\n");
    void *ptr = malloc(0);
    void *x = malloc(NULL);
    free(x);
    free(ptr);
}

// Large allocation / heap exhaustion
void test_heap_exhaustion() {
    printf("Test 7: Heap Exhaustion\n");
    void *ptrs[4096 / 8]; 
    int i = 0;
    while (i < 4096 / 8) {
        ptrs[i] = malloc(8);
        if (!ptrs[i]) break;
        i++;
    }
    printf("Allocated %d blocks before exhaustion\n", i);

    for (int j = 0; j < i; j++) free(ptrs[j]);
}

// Double free test
void test_double_free() {
    printf("Test 8: Double Free Detection\n");
    void *ptr = malloc(100);
    free(ptr);
    free(ptr);
}

// Freeing address, but not at start of chunk
void test_free_add(){
    printf("Test 9: Address Not Freed at Start of Chunk\n");
    void *p = malloc(50);
    free((char *)p + 5);
}

// Invalid free test
void test_invalid_free() {
    printf("Test 10: Invalid Free\n");
    int x;
    free(&x);
}

// Fragmentation test
void test_fragmentation() {
    printf("Test 11: Fragmentation\n");
    void *ptr1 = malloc(64);
    void *ptr2 = malloc(128);
    void *ptr3 = malloc(32);

    free(ptr2);
    void *ptr4 = malloc(100); 

    free(ptr1);
    free(ptr3);
    free(ptr4);
}

// Memory leak detection
void test_memory_leak() {
    printf("Test 12: Memory Leak Detection\n");
    void *ptr = malloc(256);
    free(ptr);
}

int main() {
    test_basic_allocation();
    test_multiple_allocation();
    test_split_chunks();
    test_alignment();
    test_coalescing();
    test_zero_allocation();
    test_heap_exhaustion();
    //test_double_free();
    //test_invalid_free();
    //test_free_add()
    test_fragmentation();
    test_memory_leak();

    printf("All tests completed.\n");
    return 0;
}
