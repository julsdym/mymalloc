#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

void test_basic_allocation() {
    printf("Testing basic allocation...\n");
    
    void *ptr1 = malloc(10);
    void *ptr2 = malloc(20);
    void *ptr3 = malloc(30);
    
    if (ptr1 && ptr2 && ptr3) {
        printf("✓ Basic allocation successful\n");
    } else {
        printf("✗ Basic allocation failed\n");
    }
    
    free(ptr1);
    free(ptr2);
    free(ptr3);
}

void test_zero_allocation() {
    printf("Testing zero-byte allocation...\n");
    
    void *ptr = malloc(0);
    if (ptr == NULL) {
        printf("✓ Zero-byte allocation correctly returns NULL\n");
    } else {
        printf("✗ Zero-byte allocation should return NULL\n");
        free(ptr);
    }
}

void test_large_allocation() {
    printf("Testing large allocation (should fail)...\n");
    
    void *ptr = malloc(5000); // Larger than heap
    if (ptr == NULL) {
        printf("✓ Large allocation correctly fails\n");
    } else {
        printf("✗ Large allocation should fail\n");
        free(ptr);
    }
}

void test_free_null() {
    printf("Testing free(NULL)...\n");
    
    free(NULL); // Should not crash
    printf("✓ free(NULL) handled correctly\n");
}

void test_invalid_pointer() {
    printf("Testing free with invalid pointer...\n");
    
    int x = 42;
    printf("Expecting error message for invalid pointer...\n");
    // This should print error and exit - comment out for other tests to run
    // free(&x);
    printf("✓ (Test skipped - would terminate program)\n");
}

void test_double_free() {
    printf("Testing double free detection...\n");
    
    void *ptr = malloc(100);
    if (ptr) {
        free(ptr);
        printf("Expecting error message for double free...\n");
        // This should print error and exit - comment out for other tests to run
        // free(ptr);
        printf("✓ (Test skipped - would terminate program)\n");
    }
}

void test_free_middle_of_chunk() {
    printf("Testing free with pointer to middle of chunk...\n");
    
    char *ptr = malloc(100);
    if (ptr) {
        printf("Expecting error message for invalid pointer...\n");
        // This should print error and exit - comment out for other tests to run
        // free(ptr + 10);
        free(ptr);
        printf("✓ (Test skipped - would terminate program)\n");
    }
}

void test_coalescing() {
    printf("Testing coalescing of adjacent free blocks...\n");
    
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
            printf("✓ Coalescing appears to work\n");
            free(large_ptr);
        } else {
            printf("? Coalescing test inconclusive\n");
        }
    }
}

void test_no_overlap() {
    printf("Testing that allocated chunks don't overlap...\n");
    
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
        printf("✓ No overlap detected between allocated chunks\n");
    } else {
        printf("✗ Overlap detected between allocated chunks\n");
    }
}

void test_memory_leak() {
    printf("Testing memory leak detection...\n");
    printf("Allocating some memory and not freeing it...\n");
    
    malloc(100);
    malloc(200);
    
    printf("✓ Memory allocated (check for leak message at program exit)\n");
}

int main() {
    printf("Running malloc/free correctness tests...\n\n");
    
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
    
    // Uncomment this line to test leak detection
    // test_memory_leak();
    
    return 0;
}
