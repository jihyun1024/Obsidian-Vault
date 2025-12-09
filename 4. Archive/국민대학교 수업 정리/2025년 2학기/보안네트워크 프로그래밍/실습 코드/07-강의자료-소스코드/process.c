#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 전역 변수
int a = 10;
int b;

void show_memory_example() {
    int x = 20;          // Stack
    int *y = malloc(sizeof(int));   // Heap

    *y = 123;

    printf("=== Memory Area Example ===\n");
    printf("전역 변수 a : %d\n", a);
    printf("전역 변수 b : %d\n", b);
    printf("지역 변수 x : %d\n", x);
    printf("동적 할당 y : %d\n", *y);

    free(y); // Heap 해제
    printf("\n");
}

void show_pid_example() {
    printf("=== Process ID ===\n");
    printf("My PID  : %d\n", getpid());
    printf("\n");
}

int main() {
    show_pid_example();
    show_memory_example();
    return 0;
}
