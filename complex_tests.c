#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "mymalloc.h"

void run_test(void (*test_func)(), const char *name) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // Child: run the test
        test_func();
        exit(0); // should not reach here if allocator exits correctly
    } else {
        // Parent: wait for child and check exit status
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);
            if (code == 2) {
                printf("[PASS] %s triggered exit(2) as expected.\n", name);
            } else {
                printf("[FAIL] %s exited with code %d\n", name, code);
            }
        } else if (WIFSIGNALED(status)) {
            printf("[FAIL] %s killed by signal %d\n", name, WTERMSIG(status));
        }
    }
}

// 12) Freeing pointer outside heap
void test_free_outside() {
    int x;
    free(&x); // expected to exit(2)
}

// 13) Freeing pointer not at start of chunk
void test_free_offset() {
    void *p = malloc(32);
    free((char*)p + 1); // expected to exit(2)
}

// 14) Double free
void test_double_free() {
    void *p = malloc(32);
    free(p);
    free(p); // expected to exit(2)
}

// 15) Freeing a pointer from system malloc
void test_system_free() {
    void *outside = (void*)0x12345678; // definitely not in heap
    myfree(outside, __FILE__, __LINE__); // should exit(2)
}


int main() {
    printf("=== Running Negative/Error Tests ===\n");

    run_test(test_free_outside, "Free outside heap");
    run_test(test_free_offset, "Free not at start of chunk");
    run_test(test_double_free, "Double free");
    run_test(test_system_free, "Free system malloc pointer");

    printf("=== Negative tests completed ===\n");
    return 0;
}
