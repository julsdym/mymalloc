#include <stdio.h>
#include <string.h>
#include "mymalloc.h"

int main(){
    //1: basic allocation
    char *p1 = malloc(100);
    if (p1) {
        printf("p1 content: %s\n", p1);
    }
    free(p1);

    //2: multiple allocations
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

    //3: split chunk
     char *p5 = malloc(50);   // should split big free chunk
    char *p6 = malloc(50);
    printf("p5=%p, p6=%p\n", (void*)p5, (void*)p6);
    free(p5);
    free(p6);

    //4: coalescing
    char *p7 = malloc(100);
    char *p8 = malloc(100);
    free(p7);
    free(p8);  
    char *p9 = malloc(200);  
    printf("p9=%p\n", (void*)p9);
    free(p9);

    //5: invalid free error (not malloced)
    /*
    int x;
    free(&x);  
    */

    //6: double free
    /*
    char *p10 = malloc(50);
    free(p10);
    free(p10); 
    */

    //7: leak check 
   /*
    char *p11 = malloc(250);
    (void)p11; 
    */

    //8: freeing address, but not at start of chunk
    /*
    void *p = malloc(50);
    free((char *)p + 5);
    */
    

    return 0;
}
