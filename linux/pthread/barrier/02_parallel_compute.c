#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000
#define THREADS 4

int data[SIZE];
int result[SIZE];
pthread_barrier_t barrier;

void *parallel_compute(void *arg) {
    int id = (int)(long)arg;
    int start = id * (SIZE / THREADS);
    int end = (id + 1) * (SIZE / THREADS);
    
    // 阶段1: 初始化数据
    for (int i = start; i < end; i++) {
        data[i] = i;
    }
    
    printf("线程 %d: 阶段1完成\n", id);
    pthread_barrier_wait(&barrier);
    
    // 阶段2: 计算平方
    for (int i = start; i < end; i++) {
        result[i] = data[i] * data[i];
    }
    
    printf("线程 %d: 阶段2完成\n", id);
    pthread_barrier_wait(&barrier);
    
    // 阶段3: 计算总和
    int sum = 0;
    for (int i = start; i < end; i++) {
        sum += result[i];
    }
    
    printf("线程 %d: 阶段3完成，局部和 = %d\n", id, sum);
    
    return NULL;
}

int main() {
    pthread_t threads[THREADS];
    
    pthread_barrier_init(&barrier, NULL, THREADS);
    
    for (int i = 0; i < THREADS; i++) {
        pthread_create(&threads[i], NULL, parallel_compute, (void *)(long)i);
    }
    
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_barrier_destroy(&barrier);
    
    return 0;
}