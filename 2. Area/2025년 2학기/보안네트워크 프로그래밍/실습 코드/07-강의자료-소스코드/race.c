#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N_THREADS   8
#define N_INCREMENTS 1000000

static long long counter = 0;

void* worker(void* arg) {
    for (int i = 0; i < N_INCREMENTS; i++) {
        // 임계구역 보호 없음 → 레이스 컨디션
        counter++;                // 읽기-수정-쓰기 사이에 끼어들기 발생 가능
    }
    return NULL;
}

int main(void) {
    pthread_t th[N_THREADS];

    for (int i = 0; i < N_THREADS; i++) {
        if (pthread_create(&th[i], NULL, worker, NULL) != 0) {
            perror("pthread_create");
            return 1;
        }
    }
    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(th[i], NULL);
    }

    printf("Expected: %lld\n", (long long)N_THREADS * N_INCREMENTS);
    printf("Actual  : %lld\n", counter);
    return 0;
}