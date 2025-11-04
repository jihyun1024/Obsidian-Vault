#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N_THREADS    8
#define N_INCREMENTS 10000

static long long counter = 0;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* worker(void* arg) {
    for (int i = 0; i < N_INCREMENTS; i++) {
        pthread_mutex_lock(&lock);
        counter++;                      // 임계구역: 상호배제 보장
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main(void) {
    pthread_t th[N_THREADS];

    // (선택) 명시적 초기화도 가능
    // pthread_mutex_init(&lock, NULL);

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

    // (선택) 명시적 파괴
    // pthread_mutex_destroy(&lock);
    return 0;
}