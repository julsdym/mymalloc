#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include "mymalloc.h"

// Global counters of tests passed and failed
static int tests_passed = 0;
static int tests_failed = 0;

// Helper macro to simplify reporting
#define REPORT(test_name, condition) \
    do { \
        if (condition) { \
            printf("%s passed!\n", test_name); \
            tests_passed++; \
        } else { \
            printf("%s failed!\n", test_name); \
            tests_failed++; \
        } \
    } while (0)

// Test 1: Malloc() reserves unallocated memory
void one(){
    size_t sizes[] = {1000, 2000, 200, 300, 250};
    unsigned char patterns[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
    void *blocks[5];
    int num_blocks = 5;

    // Allocate all blocks
    for (int i = 0; i < num_blocks; i++) {
        blocks[i] = malloc(sizes[i]);
        assert(blocks[i] != NULL);
    }

    // Fill each block with distinct pattern 
    for (int i = 0; i < num_blocks; i++) {
        unsigned char *data = (unsigned char *)blocks[i];
        for (size_t j = 0; j < sizes[i]; j++) {
        data[j] = patterns[i];
        }
    }

    // Verify each block's pattern
    bool all_ok = true;
    for (int i = 0; i < num_blocks; i++) {
        unsigned char *data = (unsigned char *)blocks[i];
        for (size_t j = 0; j < sizes[i]; j++) {
            if (data[j] != patterns[i]) {
                all_ok = false;
                break;
            }
        }
    }
    for (int i = 0; i < num_blocks; i++) {
        free(blocks[i]);
    }
    REPORT("Test: Non-overlapping Allocations", all_ok);

}

// Test 2: Free() deallocates memory and does not exceed size of heap
void two(){
    // Allocate two large blocks (should succeed)
    void *a = malloc(2000);
    void *b = malloc(2000);
    bool ok = (a != NULL && b!= NULL);

    // Try to allocate a block that exceeds remaining heap space (should fail and print error message)
    void *c = malloc(2000);
    bool allocation_failed = (c == NULL);

    // free one block and try again (should succeed)
    free(a);
    c = malloc(2000);
    bool allocation_succeeded = (c != NULL);
    free(b);
    free(c);
    REPORT("Test: Free() deallocates memory", ok && allocation_failed && allocation_succeeded);
}

// Test 3: Adjacent free blocks are coalesced
void three(){
    // 3a: mixed coalescing
    void *a = malloc(100);
    void *b = malloc(100);
    void *c = malloc(100);

    assert(a && b && c);

    free(a);
    free(c);
    free(b);

    void *d = malloc(300);
    assert(d != NULL);
    char *dd = (char *)d;
    char *aa = (char *)a;
    char *cc = (char *)c;
    REPORT("Test: Basic coalescing", (dd >= aa && dd <= cc + 100));
    free(d);
   

    // 3b: Left/right coalescing
    void *blocks[5];
    for (int i = 0; i < 5; i++) {
        blocks[i] = malloc(100);
        assert(blocks[i]);
    }
    free(blocks[1]);
    free(blocks[2]);
    
    void *right_coalesce = malloc(180);
    bool right_ok = (right_coalesce != NULL);
    if (right_coalesce) free(right_coalesce);
     free(blocks[4]);
    free(blocks[3]);
    
    void *left_coalesce = malloc(180);
    bool left_ok = (left_coalesce != NULL);
    if (left_coalesce) free(left_coalesce);
    
    REPORT("Test: Left and right coalescing", (right_ok && left_ok));

    free(blocks[0]);

    // 3c: Alternate coalescing
    const int pairs = 10;
    void *small_blocks[pairs];
    void *large_blocks[pairs];
    
    for (int i = 0; i < pairs; i++) {
        small_blocks[i] = malloc(8);
        large_blocks[i] = malloc(10);
        
        if (!small_blocks[i] || !large_blocks[i]) {
            REPORT("Test: Allocation", false); 
            for (int j = 0; j <= i; j++) {
                if (small_blocks[j]) free(small_blocks[j]);
                if (large_blocks[j]) free(large_blocks[j]);
            }
            return;
        }
        
        memset(small_blocks[i], 0xAA, 8);
        memset(large_blocks[i], 0xBB, 10);
    }
    
    bool all_ok = true;
    for (int i = 0; i < pairs; i++) {
        unsigned char *small = (unsigned char*)small_blocks[i];
        unsigned char *large = (unsigned char*)large_blocks[i];
        
        for (int j = 0; j < 8; j++) {
            if (small[j] != 0xAA) {
                all_ok = false;
                break;
            }
        }
        
        for (int j = 0; j < 10; j++) {
            if (large[j] != 0xBB) {
                all_ok = false;
                break;
            }
        }
    }
    
   REPORT("Test: Alternating Allocations", all_ok);
    for (int i = 0; i < pairs; i++) {
        free(small_blocks[i]);
        free(large_blocks[i]);
    }
}

// Test 4:  Error Detection - calling free() with address not from malloc()
void four(){
    int x = 42;
    free(&x); // Should print error message
}

// Test 5: Error Detection - free() with address not at chunk start
void five(){
    void *p = malloc(100);
    assert(p != NULL);
    void *mid = (char*)p + 10;
    free(mid); // Should print error message
    free(p);
}

// Test 6: Error Detection - double free of same address
void six(){
    void *p = malloc(100);
    assert(p != NULL);
    free(p);
    free(p); // Should print error message  
}

// Test 7: Heap full 
void seven() {
    // Try to allocate progressively until failure
    void *ptrs[100];
    int i = 0;

    // Allocate until NULL
    while (i < 100 && (ptrs[i] = malloc(500)) != NULL) {
    i++;
    }

    int allocated = i;
    printf("Successfully allocated %d blocks of 500 bytes\n", allocated);

    // Next allocation should fail
    void *fail = malloc(500);
    REPORT("malloc() returns NULL when out of memory", fail ==NULL);
    printf("(Should see error message above about being unable to allocate)\n");
    for (int j = 0; j < allocated; j++) {
        free(ptrs[j]);
    }

}

// Test 8: Error Detection - memory leak detection at program exit
void eight(){
    malloc(1000); // Should print leak message at program exit
}

// Helper to get header from payload pointer
typedef struct header {
    size_t size;
    int is_free;
} header;

static header *get_header(void *p) {
    return (header *)((char *)p - sizeof(header));
}

// Test 9: 8-Byte alignment of allocated blocks + Minimum chunk size is 16 bytes
void nine(){
    size_t test_sizes[] = {1, 7, 8, 9, 20, 32, 33};
    bool ok = true;
    bool two = true;
    for (int i = 0; i < 7; i++) {
        void *p = malloc(test_sizes[i]);
        assert(p != NULL);
        // Check 8-byte alignment
        if (((uintptr_t)p % 8) != 0) {
            ok = false;
            continue;
        }
        header *h = get_header(p);
        size_t total_chunk = h->size + sizeof(header);
        if (total_chunk < 16) {
           two = false;
        }
        free(p);
    }
    REPORT("Test: 8-byte alignment", (ok && two));
}

// Test 10: Null and Zero Behavior
void ten(){
    void *p1 = malloc(0);
    free(NULL); 
    REPORT("Test: Null & Zero Behavior", p1 == NULL);
}

// Test 11: Contents of allocated object do not change
void eleven(){

    // Payload remains unchanged
    char *p = malloc(16);
    strcpy(p, "Hello");

    void *q = malloc(32);
    void *r = malloc(64);
    free(q);
    free(r);
    bool ok = (strcmp(p, "Hello") == 0);
    REPORT("Test: Contents unchanged", ok);
    free(p);

}

int main(int argc, char *argv[]) {
    one();
    two();
    three();
    nine();
    ten();
    eleven();
    printf("Number of tests passed: %d\n", tests_passed);
    printf("Number of tests failed: %d\n", tests_failed);
    printf("All safe tests completed.\n");
    printf("To test error cases, run: ./test double | invalid | mid | leak | full\n");
    
    // Error tests to be run individually since they exit(2)
    if (argc > 1) {
        if (strcmp(argv[1], "double") == 0) six();
        if (strcmp(argv[1], "invalid") == 0) four();
        if (strcmp(argv[1], "mid") == 0) five();
        if (strcmp(argv[1], "leak") == 0) eight();
        if (strcmp(argv[1], "full") == 0) seven();
    }
    return 0;
}
