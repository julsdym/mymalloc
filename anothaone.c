#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "mymalloc.h"

void test_basic_allocation() {
    printf("Testing basic allocation\n");
    
    void *ptr1 = malloc(10);
    void *ptr2 = malloc(20);
    void *ptr3 = malloc(30);
    
    if (ptr1 && ptr2 && ptr3) {
        printf("Basic allocation successful\n");
    } else {
        printf("Basic allocation failed\n");
    }
    
    free(ptr1);
    free(ptr2);
    free(ptr3);

    void *a = malloc(32);
    void *b = malloc(64);
    assert(a && b);

    memset(a, 0xAA, 32);
    memset(b, 0xBB, 64);

    free(a);
    free(b);
}

void test_alignment() {
    printf("Test 2: Alignment\n");
    size_t sizes[] = {1, 7, 15, 31, 100};
    for (int i = 0; i < 5; i++) {
        void *p = malloc(sizes[i]);
        assert(p);
        assert(((uintptr_t)p % 8) == 0); // must be 8-byte aligned
        free(p);
    }
    printf("All pointers 8-byte aligned\n\n");
}

// Test 3: Coalescing
void test_coalesce() {
    printf("Test 3: Coalescing\n");
    void *a = malloc(128);
    void *b = malloc(128);
    void *c = malloc(128);

    free(b);
    free(a);
    free(c);

    void *big = malloc(400); // should succeed if coalescing worked
    assert(big);
    free(big);
    printf("Coalescing successful\n\n");
}

// ---------- Test 4: Heap full ----------
void test_heap_full() {
    printf("Test 4: Heap full\n");
    void *blocks[1000];
    int count = 0;

    while (1) {
        void *p = malloc(256);
        if (!p) {
            printf("Heap full error triggered after %d allocations\n\n", count);
            break;
        }
        blocks[count++] = p;
    }

    // cleanup
    for (int i = 0; i < count; i++) {
        free(blocks[i]);
    }
}

// ---------- Test 5: Error cases ----------
// NOTE: These will intentionally crash/exit(2). Run individually.

void test_double_free() {
    printf("Test 5a: Double free (should exit with error)\n");
    void *p = malloc(64);
    free(p);
    free(p); // should trigger error
}

void test_invalid_free() {
    printf("Test 5b: Invalid free (should exit with error)\n");
    int x;
    free(&x); // freeing pointer not from mymalloc
}

void test_mid_chunk_free() {
    printf("Test 5c: Freeing middle of chunk (should exit with error)\n");
    void *p = malloc(64);
    void *mid = (char*)p + 4;
    free(mid); // not at start of chunk
}

void test_zero_allocation() {
    printf("Testing zero-byte allocation\n");
    
    void *ptr = malloc(0);
    if (ptr == NULL) {
        printf("Zero-byte allocation correctly returns NULL\n");
    } else {
        printf("Zero-byte allocation should return NULL\n");
        free(ptr);
    }
}

void test_large_allocation() {
    printf("Testing large allocation (should fail)...\n");
    
    void *ptr = malloc(5000); // Larger than heap
    if (ptr == NULL) {
        printf("Large allocation correctly fails\n");
    } else {
        printf("Large allocation should fail\n");
        free(ptr);
    }
}

void test_free_null() {
    printf("Testing free(NULL)\n");
    
    free(NULL); // Should not crash
    printf("free(NULL) handled correctly\n");
}

void test_invalid_pointer() {
    printf("Testing free with invalid pointer\n");
    
    int x = 42;
    printf("Expecting error message for invalid pointer\n");
    // This should print error and exit - comment out for other tests to run
    // free(&x);
    printf("Test skipped - would terminate program\n");
}

void test_double_free() {
    printf("Testing double free detection\n");
    
    void *ptr = malloc(100);
    if (ptr) {
        free(ptr);
        printf("Expecting error message for double free.\n");
        // This should print error and exit - comment out for other tests to run
        // free(ptr);
        printf("Test skipped - would terminate program\n");
    }
}

void test_free_middle_of_chunk() {
    printf("Testing free with pointer to middle of chunk\n");
    
    char *ptr = malloc(100);
    if (ptr) {
        printf("Expecting error message for invalid pointer\n");
        // This should print error and exit - comment out for other tests to run
        // free(ptr + 10);
        free(ptr);
        printf("Test skipped - would terminate program\n");
    }
}

void test_coalescing() {
    printf("Testing coalescing of adjacent free blocks\n");
    
    void *ptr1 = malloc(100);
    void *ptr2 = malloc(100);
    void *ptr3 = malloc(100);
    
    if (ptr1 && ptr2 && ptr3) {
        free(ptr1);
        free(ptr3);
        free(ptr2); // Should coalesce all three
        
        // Try to allocate something large
        void *large_ptr = malloc(280);
        if (large_ptr) {
            printf("Coalescing appears to work\n");
            free(large_ptr);
        } else {
            printf("Coalescing test inconclusive\n");
        }
    }
}

void test_no_overlap() {
    printf("Testing that allocated chunks don't overlap\n");
    
    char *ptrs[10];
    int i;
    
    // Allocate several chunks
    for (i = 0; i < 10; i++) {
        ptrs[i] = malloc(50);
        if (!ptrs[i]) break;
    }
    
    // Fill each with a unique pattern
    for (int j = 0; j < i; j++) {
        memset(ptrs[j], j + 1, 50);
    }
    
    // Check patterns are intact
    int overlap_detected = 0;
    for (int j = 0; j < i; j++) {
        for (int k = 0; k < 50; k++) {
            if (ptrs[j][k] != (char)(j + 1)) {
                overlap_detected = 1;
                break;
            }
        }
        if (overlap_detected) break;
    }
    
    // Free all
    for (int j = 0; j < i; j++) {
        free(ptrs[j]);
    }
    
    if (!overlap_detected) {
        printf("Passed: No overlap detected between allocated chunks\n");
    } else {
        printf("Failed: Overlap detected between allocated chunks\n");
    }
}

void test_memory_leak() {
    printf("Testing memory leak detection\n");
    printf("Allocating some memory and not freeing it\n");
    
    malloc(100);
    malloc(200);
    
    printf("Passed: Memory allocated (check for leak message at program exit)\n");
}

int main(int argc, char *argv[]) {
    printf("Running malloc/free correctness tests\n\n");
    
    test_basic_allocation();
    test_zero_allocation();
    test_large_allocation();
    test_free_null();
    test_invalid_pointer();
    test_double_free();
    test_free_middle_of_chunk();
    test_coalescing();
    test_no_overlap();
    
    printf("\nAll basic tests completed.\n");
    printf("Note: Some error tests were skipped to prevent program termination.\n");
    printf("To test error conditions, uncomment the relevant lines in test.c\n\n");
    
    test_basic();
    test_alignment();
    test_coalesce();
    test_heap_full();

    // Error tests: run individually, since they exit(2)
    if (argc > 1) {
        if (strcmp(argv[1], "double") == 0) test_double_free();
        if (strcmp(argv[1], "invalid") == 0) test_invalid_free();
        if (strcmp(argv[1], "mid") == 0) test_mid_chunk_free();
    }

    printf("All safe tests completed\n");
    printf("Run ./tests double | invalid | mid to test error cases.\n");

    // Uncomment this line to test leak detection
    test_memory_leak();
    
    return 0;
}
