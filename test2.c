#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "malloc.h"

#define HEAP_SIZE 4096

// Helper macro for alignment check
#define IS_ALIGNED(ptr, alignment) (((uintptr_t)(ptr) % (alignment)) == 0)

// Basic allocation and free
void test_basic_allocation() {
    printf("Test 1: Basic Allocation/Free\n");
    void *ptr = malloc(100);
    assert(ptr != NULL);
    free(ptr);
}

// Alignment test
void test_alignment() {
    printf("Test 2: 8-byte Alignment\n");
    for (int i = 1; i <= 256; i *= 2) {
        void *ptr = malloc(i);
        assert(ptr != NULL);
        assert(IS_ALIGNED(ptr, 8));
        free(ptr);
    }
}

// Coalescing test
void test_coalescing() {
    printf("Test 3: Coalescing Adjacent Blocks\n");
    void *a = malloc(100);
    void *b = malloc(200);
    void *c = malloc(150);

    free(a);
    free(c);
    free(b); // After freeing b, a, b, c should coalesce
    // Optional: check internal free memory if tracked
}

// Zero-byte allocation
void test_zero_allocation() {
    printf("Test 4: Zero-byte Allocation\n");
    void *ptr = malloc(0);
    // Implementation dependent: may return NULL or a valid pointer
    free(ptr);
}

// Large allocation / heap exhaustion
void test_heap_exhaustion() {
    printf("Test 5: Heap Exhaustion\n");
    void *ptrs[HEAP_SIZE / 8]; // Max possible small allocations
    int i = 0;
    while (i < HEAP_SIZE / 8) {
        ptrs[i] = malloc(8);
        if (!ptrs[i]) break;
        i++;
    }
    printf("Allocated %d blocks before exhaustion\n", i);

    for (int j = 0; j < i; j++) free(ptrs[j]);
}

// Double free test
void test_double_free() {
    printf("Test 6: Double Free Detection\n");
    void *ptr = malloc(100);
    free(ptr);
    // Depending on implementation, this may cause error/warning
    free(ptr);
}

// Invalid free test
void test_invalid_free() {
    printf("Test 7: Invalid Free\n");
    int dummy;
    // Freeing non-heap pointer
    free(&dummy);
}

// Fragmentation test
void test_fragmentation() {
    printf("Test 8: Fragmentation\n");
    void *ptr1 = malloc(64);
    void *ptr2 = malloc(128);
    void *ptr3 = malloc(32);

    free(ptr2); // free middle
    void *ptr4 = malloc(100); // should fit if coalescing works correctly

    free(ptr1);
    free(ptr3);
    free(ptr4);
}

// Memory leak detection
void test_memory_leak() {
    printf("Test 10: Memory Leak Detection\n");
    void *ptr = malloc(256);
    free(ptr);
    // If your malloc tracks total free memory, verify it returns to full heap
}

int main() {
    test_basic_allocation();
    test_alignment();
    test_coalescing();
    test_zero_allocation();
    test_heap_exhaustion();
    //test_double_free();
    //test_invalid_free();
    test_fragmentation();
    test_memory_leak();

    printf("All tests completed.\n");
    return 0;
}
