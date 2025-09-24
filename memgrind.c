#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "mymalloc.h"


//function to get current time in milliseconds
long get_time(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

//Task 1: allocate 120 1-byte objects then immediately free
void task1(){
    for(int i=0; i<120;i++){
        void *p = malloc(1);
        free(p);
    }
}

//Task 2: allocate 120 1-byte objects, store pointers then free all
void task2(){
    void *p[120];
    for(int i=0; i<120;i++){
        p[i] = malloc(1);
    }
    for(int i=0; i<120;i++){
        free(p[i]);
    }
}

//Task 3: Random allocation/deallocation until 120 allocations done
void task3(){
   void *p[120];
   int count = 0;
    int total_allocated = 0;
    for(int i=0;i<120;i++){
        p[i] = NULL;
    }

    //random choice: 0 to allocate, 1 to free
    while (total_allocated < 120){
        int choice = rand() % 2;
        if(choice ==0 && count < 120){
            for (int i=0; i<120;i++){
                if(p[i] == NULL){
                    p[i] = malloc(1);
                    count++;
                    total_allocated++;
                    break;
                }
            }
        } else if (choice ==1 && count > 0){
            int start = rand() % 120;
            for(int i=0; i< 120;i++){
                int one = (start + i) % 120;
                if (p[one]!= NULL){
                    free(p[one]);
                    p[one] = NULL;
                    count--;
                    break;
                }
            }
        }
    }
    // free rest of allocated objects
    for(int i=0;i<120;i++){
        if(p[i] != NULL){
            free(p[i]);
        }
    }
}

//Task 4: Linked List operations
void task4(){
   typedef struct node{
        int data;
        struct node *next;
   } node;

   //build linked list of 60 nodes
   node *head = NULL;
   for (int i=0;i<60;i++){
        node *new = malloc(sizeof(node));
        new->data = i;
        new->next = head;
        head = new;
   }

   //remove every other node
   node *current = head;
   while (current != NULL && current->next != NULL){
        node *del = current->next;
        current->next = del->next;
        free(del);
        current = current->next;
   }

   //add 30 more nodes
   for(int i=0; i<30;i++){
        node *new = malloc(sizeof(node));
        new->data = i+100;
        new->next = head;
        head = new;
   }

   //free remaining nodes
   while (head != NULL){
        node *temp = head;
        head = head->next;
        free(temp);
   }
}

//Task 5: Binary Tree Operations
void task5(){
    typedef struct tree_node{
        int val;
        struct tree_node *left;
        struct tree_node *right;
    } tree_node;

    tree_node *node[40];

    //initialize tree nodes 
    for(int i=0;i<40;i++){
        node[i] = malloc(sizeof(tree_node));
        node[i]->val = i;
        node[i]->left = NULL;
        node[i]->right = NULL;
    }

    //build binary tree
    for (int i=20;i<30;i++){
        free(node[i]);
        node[i] = malloc(sizeof(tree_node));
        node[i]->val = i+1000;
        node[i]->right = NULL;
        node[i]->left = NULL;
    }

    for(int i=0; i<40; i++){
        free(node[i]);
    }
   
}

//run each task once
void run_workload_once(){
    task1();
    task2();
    task3();
    task4();
    task5();
}

int main(){
    srand((unsigned)time(NULL));
    //run the workload 50 times
    long start = get_time();
    for (int i=0; i<50;i++){
        run_workload_once();
    }
    long end = get_time();
    long total = (end - start) / (long) 50;
    printf("memgrind: Average time over %d runs: %ld seconds\n", 50, total);
    return 0;
}
