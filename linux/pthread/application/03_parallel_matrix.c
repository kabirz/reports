#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000
#define THREADS 4

int matrix_a[SIZE][SIZE];
int matrix_b[SIZE][SIZE];
int matrix_c[SIZE][SIZE];

typedef struct {
    int start_row;
    int end_row;
} thread_data_t;

void *multiply(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < SIZE; j++) {
            matrix_c[i][j] = 0;
            for (int k = 0; k < SIZE; k++) {
                matrix_c[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }
    
    return NULL;
}

int main() {
    pthread_t threads[THREADS];
    thread_data_t data[THREADS];
    struct timespec start, end;
    double time_parallel, time_sequential;
    
    // 初始化矩阵
    srand(time(NULL));
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            matrix_a[i][j] = rand() % 100;
            matrix_b[i][j] = rand() % 100;
        }
    }
    
    // 并行乘法
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < THREADS; i++) {
        data[i].start_row = i * (SIZE / THREADS);
        data[i].end_row = (i + 1) * (SIZE / THREADS);
        pthread_create(&threads[i], NULL, multiply, &data[i]);
    }
    
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_parallel = (end.tv_sec - start.tv_sec) + 
                    (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("并行乘法: %.3f 秒\n", time_parallel);
    
    // 顺序乘法
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            matrix_c[i][j] = 0;
            for (int k = 0; k < SIZE; k++) {
                matrix_c[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_sequential = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("顺序乘法: %.3f 秒\n", time_sequential);
    printf("加速比: %.2fx\n", time_sequential / time_parallel);
    
    return 0;
}