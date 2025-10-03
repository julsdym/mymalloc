Julia Dymnicki 
netid jd1604

Test Case 1: malloc() reserves unallocated memory
Requirement: malloc() reserves unallocated memory. The payload does not overlap any other allocated chunks. 
Detection method: When successful, malloc() returns a pointer to an object that does not overlap with any other allocated object.
Test: Write a program that allocates several large objects. Once allocation is complete, it fills each object with a distinct byte pattern (e.g., the first object is filled with 1, the second with 2, etc.). Finally, it checks whether each object still contains the written pattern. (That is, writing to one object did not overwrite any other.)

Test Case 2: free() deallocates memory
Requirement: free() deallocates memory 
Detection method: After allocating a variety of large objects, check to make sure that free() works by seeing if you can continue to allocate memory until it hits the size of the heap (4096). If you allocate two large objects that are each half the size of the heap, then free them both and attempt to allocate another large object, then free() works as it deallocates those objects from the heap.
Test: Write a program that allocates several large objects that are close to the maximum size of the heap.  Try to allocate another object which would exceed the size of the heap, verify that the program prints an error message, and empty one of the first few objects and try to allocate memory for the last object again.  If it can be allocated, then free() deallocates memory from the heap.

Test Case 3: Adjacent free blocks are coalesced: Small chunks can coalesce into larger chunks.
Requirement: malloc() and free() arrange so that adjacent free blocks are coalesced into larger contiguous free blocks.
Detection method: After freeing adjacent blocks, they should merge into a single larger free block. This can be detected by verifying that a following allocation requiring the combined size succeeds, whereas it would fail if the blocks remained fragmented.
Test: Write a program that allocates three consecutive blocks of the same size, then free block A, C, B.  Then, attempt to allocate a block of size equal to A + B + C combined and if malloc() doesn't return an error, then it successfully allocated memory for the large block, proving that the three adjacent free blocks were coalesced into one contiguous block. It then tests different variations of coalescing by varying the order of frees.

Test Case 4: Error Detection - Call Free() with address not from malloc()
Requirement: Detect and report when free() is called with an address not obtained from malloc().
Detection method: If the pointer being freed does not correspond to any valid allocated chunk, the program will print an error message in the format: free: Inappropriate pointer (source.c:LINE), then exit with status 2.
Test: Write a program that declares a variable (int x;), attempts to free the address of that variable (free(&x);), then verifies that the appropriate error message to stderr is printed and exits with status 2. 

Test Case 5: Error Detection - Free() with address not at chunk start
Requirement: Detect and report when free() is called with an address that is not at the start of an allocated chunk.
Detection method: If the pointer being freed points to an offset within an allocated chunk rather than to its beginning, the program will print an error message in the format: free: Inappropriate pointer (source.c:LINE), then exit with status 2.
Test: Write a program that allocates an array (e.g., int *p = malloc(sizeof(int) * 2);), then attempts to free a pointer to the middle of the array (e.g., free(p + 1);). Verify that the program prints the appropriate error message to stderr and exits with status 2.  

Test Case 6: Error Detection - Double free
Requirement: Detect and report when free() is called a second time on the same pointer.
Detection method: When a pointer to an already-freed chunk is passed to free() again, an error message should be printed in the format: free: Inappropriate pointer (source.c:LINE), then exit with status 2.
Test: Write a program that allocates a block of memory (int *p = malloc(sizeof(int) * 100);), creates a variable to the same pointer (int *q = p;), frees the block using the first pointer (free(p);), then attempts to free the block again using the new variable (free(q);). The appropriate error message should be printed to stderr and exit with status 2.  

Test Case 7: Error Detection - malloc() unable to fulfill request (Heap Full)
Requirement: When malloc() cannot fulfill an allocation request, it returns NULL and prints an error message.
Detection method: malloc() should return NULL when insufficient memory is available and print a message to stderr in the format: malloc: Unable to allocate N bytes (source.c:LINE).
Test: Write a program that repeatedly allocates memory until it surpasses the size of heap, then attempts one more allocation that should fail.  It should verify that malloc() returns NULL and that an appropriate error message is printed to stderr with the correct byte count, filename, and line number.  It should also verify that the program can continue execution after the failed allocation.

Test Case 8: Memory leak detection
Requirement: Leaked objects (allocated but never freed) are detected and reported.
Detection method: At program termination, malloc should identify all allocated blocks that were never freed and print a message to standard error indicating the number of allocated chunks and their size in bytes.
Test: Write a program that allocates several blocks of memory of varying sizes, frees some of the blocks but leaves others allocated. Before program termination, malloc() reports the leaked memory, including the number of leaked blocks, total bytes leaked, and file and line number where the leaked allocation occurred. 

Test Case 9: 8-Byte Alignment of allocated memory
Requirement: An object allocated by malloc() has 8-byte alignment: each chunk must have a length that is a multiple of 8, and the smallest possible chunk is 16 bytes.  The pointer returned by malloc() must point to the payload, not the chunk header. Pointers are properly 8-byte aligned.
Detection method: For each object allocated, verify that the address returned by malloc() is divisible by 8 and that the pointer is greater than or equal to the starting address of the heap plus the size of the header (smallest chunk size is 16 bytes).
Test: Write a program that allocates memory for varying types of varying sizes (e.g., double, long long) and check that the allocated payload size is the next multiple of 8 ≥ requested size (e.g., malloc(1) = payload 8 bytes, total chunk 16. malloc(20) =  payload 24 bytes, total chunk 32. malloc(32) = payload 32 bytes, total chunk 40.)

Test Case 10: Null & Free Behavior
Requirement: Free is able to free a null, malloc should not crash when asked to allocate 0 bytes.
Detection Method: Attempt to free(NULL) and malloc(0). Should return null pointer for malloc(0).
Test: Write a program that writes free(NULL) and malloc(0).

Test Case 11: Data and Metadata are Separate and Safe
Requirement: The run-time system makes no assumptions about the data written to the payload of a chunk. The run-time cannot extract any information by looking at the payload of an allocated chunk. Clients may write to any byte received from malloc() without causing problems for the run-time system.  Any data it writes to chunk headers or to the payloads of unallocated chunks will be not be read or updated by client code.
Detection Method: Create a variable, put information in that variable, check to see if it stays the same after allocating and freeing other blocks.  Chunks are separate and the system does not touch the payload / data of chunks.
Test: Write a program that allocates memory for a variable, puts data in that variable, then allocates other objects and frees them, then checks to see if the original data remains unchanged and the system did not touch the payload of the data.

// Note: Tests four, five, six, seven, eight need to be ran with "./test double" "./test invalid" "./test mid" "./test leak" "./test full" in order to be tested as they will cause an error message and for the program to exit.
To run test 4: type in "./test invalid"
To run test 5: type in "./test mid"
To run test 6: type in "./test double"
To run test 7: type in "./test full"
To run test 8: type in "./test leak"









