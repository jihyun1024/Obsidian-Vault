#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// 전역 변수
int a = 10;
int b;

int main(void) {
    int x = 20;                // Stack
    int *y = malloc(sizeof(int)); // Heap
    *y = 123;

    printf("=== BEFORE fork ===\n");
    printf("PID=%d | a=%d, b=%d, x=%d, *y=%d\n\n", getpid(), a, b, x, *y);

    pid_t pid = fork();

    if (pid == 0) {
        // 자식 프로세스: 자기 복사본만 변경
        a += 1; b += 1; x += 1; (*y) += 1;
        printf("[Child ] PID=%d | a=%d, b=%d, x=%d, *y=%d\n",
               getpid(), a, b, x, *y);
        free(y);
        _exit(0);
    } else {
        // 부모 프로세스: 자기 복사본만 변경
        a += 10; b += 10; x += 10; (*y) += 10;
        printf("[Parent] PID=%d | a=%d, b=%d, x=%d, *y=%d\n",
               getpid(), a, b, x, *y);
        wait(NULL);   // 종료 대기
        free(y);
    }
    return 0;
}