Julia Dymnicki 
netid jd1604

Test Case 1: malloc() reserves unallocated memory
Requirement: malloc() reserves unallocated memory.
Detection method: When successful, malloc() returns a pointer to an object that does not overlap with any other allocated object.
Test: Write a program that allocates several large objects. Once allocation is complete, it fills each object with a distinct byte pattern (e.g., the first object is filled with 1, the second with 2, etc.). Finally, it checks whether each object still contains the written pattern. (That is, writing to one object did not overwrite any other.)

Test Case 2: free() deallocates memory
Requirement: free() deallocates memory 
Detection method: After allocating a variety of large objects, check to make sure that free() works by seeing if you can continue to allocate memory until it hits the size of the heap. If you allocate two large objects that are each half the size of the heap, then free them both and attempt to allocate another large object, then free() works as it deallocates those objects from the heap.
Test: Write a program that allocates several large objects that are close to the maximum size of the heap.  Try to allocate another object which would exceed the size of the heap, verify that the program prints an error message, and empty one of the first few objects and try to allocate memory for the last object again.  Verify that you are able to allocate it. If so, free() deallocates memory from the heap!

Test Case 3: Adjacent free blocks are coalesced
Requirement: malloc() and free() arrange so that adjacent free blocks are coalesced into larger contiguous free blocks to prevent fragmentation.
Detection method: After freeing adjacent blocks, the memory manager should merge them into a single larger free block. This can be detected by verifying that a subsequent allocation requiring the combined size succeeds, whereas it would fail if the blocks remained fragmented.
Test: Write a program that allocates three consecutive blocks (A, B, C) of known sizes (e.g., 100 bytes each) then frees block A, then frees block C, then frees block B (which is between A and C).  Then, it attempts to allocate a block of size equal to A + B + C combined and verifies that this large allocation succeeds, proving that the three adjacent free blocks were coalesced into one contiguous block. It then tests both forward and backward coalescing by varying the order of frees.

Test Case 4: Error Detection - calling free() with address not from malloc()
Requirement: Detect and report when free() is called with an address not obtained from malloc().
Detection method: The library should recognize that the pointer being freed does not correspond to any valid allocated chunk and print an error message in the format: free: Inappropriate pointer (source.c:LINE), then exit with status 2.
Test: Write a program that declares a variable (e.g., int x;), attempts to free the address of that variable (free(&x);), then verifies that the program prints the appropriate error message to stderr and exits with status 2. To test additional cases, I will attempt to free NULL, free addresses from the middle of the heap, and free global variables.

Test Case 5: Error Detection - free() with address not at chunk start
Requirement: Detect and report when free() is called with an address that is not at the start of an allocated chunk.
Detection method: The library should recognize that the pointer being freed points to an offset within an allocated chunk rather than to its beginning, print an error message in the format: free: Inappropriate pointer (source.c:LINE), then exit with status 2.
Test: Write a program that allocates an array (e.g., int *p = malloc(sizeof(int) * 10);), then attempts to free a pointer to the middle of the array (e.g., free(p + 5);).  I will verify that the program prints the appropriate error message to stderr and exits with status 2.  In order to test edge cases, I will free one byte past the start and free at the last byte of the chunk.

Test Case 6: Error Detection - Double free
Requirement: Detect and report when free() is called a second time on the same pointer.
Detection method: The library should track which chunks have been freed and recognize when a pointer to an already-freed chunk is passed to free() again. It should print an error message in the format: free: Inappropriate pointer (source.c:LINE), then exit with status 2.
Test: Write a program that allocates a block of memory (int *p = malloc(sizeof(int) * 100);), creates a variable to the same pointer (int *q = p;), frees the block using the first pointer (free(p);), then attempts to free the block again using the new variable (free(q);). It will verify that the program prints the appropriate error message to stderr and exits with status 2.  I will also test different variations such as a double free with the same pointer variable, and triple free attempts.

Test Case 7: Error Detection - malloc() unable to fulfill request (Heap Full)
Requirement: When malloc() cannot fulfill an allocation request, it returns NULL and prints an error message.
Detection method: malloc() should return NULL when insufficient memory is available and print a message to stderr in the format: malloc: Unable to allocate N bytes (source.c:LINE).
Test: Write a program that repeatedly allocates memory until the heap is exhausted, then attempts one more allocation that should fail.  It should verify that malloc() returns NULL and that an appropriate error message is printed to stderr with the correct byte count, filename, and line number.  It should also verify that the program can continue execution after the failed allocation.  Other edge cases I will test will include requesting more memory than the total heap size and requesting very large allocations.

Test Case 8: Memory leak detection
Requirement: Leaked objects (allocated but never freed) are detected and reported.
Detection method: At program termination, malloc should identify all allocated blocks that were never freed and print a message to standard error indicating the number of allocated chunks and their size in bytes.
Test: Write a program that allocates several blocks of memory of varying sizes, frees some of the blocks but leaves others allocated. Before program termination, I will verify that malloc() reports the leaked memory, including the number of leaked blocks, total bytes leaked, and file and line number where the leaked allocation occurred. 

Test Case 9: 8-Byte Alignment of allocated memory
Requirement: An object allocated by malloc() has 8-byte alignment: each chunk must have a length that is a multiple of 8, and the smallest possible chunk is 16 bytes.  The pointer returned by malloc() must point to the payload, not the chunk header.
Detection method: For each object allocated, verify that the address returned by malloc() is divisible by 8 and that the pointer is greater than or equal to the starting address of the heap plus the size of the header (smallest chunk size is 16 bytes).
Test: Write a program that allocates memory for varying types of varying sizes (e.g., double, long long) and check that the allocated payload size is the next multiple of 8 ≥ requested size (e.g., malloc(1) = payload 8 bytes, total chunk 16. malloc(20) =  payload 24 bytes, total chunk 32. malloc(32) = payload 32 bytes, total chunk 40.)

Test Case 10: Null & Free Behavior
Requirement: Free is able to free a null, malloc should not crash when asked to allocate 0 bytes.
Detection Method: Attempt to free(NULL) and malloc(0). Should return null pointer for malloc(0).
Test: Write a program that writes free(NULL) and malloc(0).







