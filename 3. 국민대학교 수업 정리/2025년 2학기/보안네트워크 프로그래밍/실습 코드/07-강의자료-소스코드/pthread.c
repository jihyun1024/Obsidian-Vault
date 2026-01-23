#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

// 전역(공유) 변수
int a = 10;
int b;
int *z; // Heap(공유)

void* worker(void *arg) {
    int id = (int)(intptr_t)arg;     // 스레드 식별용 1 또는 2
    int x = 100 + id;                // 각 스레드의 Stack 값 다르게
    a += 1;                          // 공유 (두 스레드가 함께 변경)
    b += 1;                          // 공유
    (*z) += 1;                    // 공유

    printf("[Thread %d] tid=%lu | x=%d , &a=%p, *z=%d\n",
           id, (unsigned long)pthread_self(), x,
           (void*)&a, *z);
    return NULL;
}

int main(void) {
    int y = 20;                 // main의 Stack
    z = malloc(sizeof(int)); // Heap(공유)
    *z = 123;

    printf("=== BEFORE threads ===\n");
    printf("a=%d, b=%d, y=%d, *z=%d\n\n", a, b, y, *z);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, worker, (void*)(intptr_t)1);
    pthread_create(&t2, NULL, worker, (void*)(intptr_t)2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\n=== AFTER threads ===\n");
    printf("a=%d, b=%d, y=%d, *z=%d\n", a, b, y, *z);

    free(z);
    return 0;
}
