#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10000
#define THREADS 4

int array[SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void merge(int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    int L[n1], R[n2];
    
    for (int i = 0; i < n1; i++)
        L[i] = array[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = array[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            array[k] = L[i];
            i++;
        } else {
            array[k] = R[j];
            j++;
        }
        k++;
    }
    
    while (i < n1) {
        array[k] = L[i];
        i++;
        k++;
    }
    
    while (j < n2) {
        array[k] = R[j];
        j++;
        k++;
    }
}

void merge_sort(int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(left, mid);
        merge_sort(mid + 1, right);
        merge(left, mid, right);
    }
}

void *parallel_merge_sort(void *arg) {
    int id = (int)(long)arg;
    int start = id * (SIZE / THREADS);
    int end = (id + 1) * (SIZE / THREADS) - 1;
    
    merge_sort(start, end);
    
    return NULL;
}

int main() {
    pthread_t threads[THREADS];
    struct timespec start, end;
    double time_parallel, time_sequential;
    
    // 初始化数组
    srand(time(NULL));
    for (int i = 0; i < SIZE; i++) {
        array[i] = rand() % 10000;
    }
    
    // 并行排序
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < THREADS; i++) {
        pthread_create(&threads[i], NULL, parallel_merge_sort, (void *)(long)i);
    }
    
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // 合并结果
    for (int i = 1; i < THREADS; i++) {
        merge(0, i * (SIZE / THREADS) - 1, (i + 1) * (SIZE / THREADS) - 1);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_parallel = (end.tv_sec - start.tv_sec) + 
                    (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("并行排序: %.3f 秒\n", time_parallel);
    
    // 重新初始化数组
    for (int i = 0; i < SIZE; i++) {
        array[i] = rand() % 10000;
    }
    
    // 顺序排序
    clock_gettime(CLOCK_MONOTONIC, &start);
    merge_sort(0, SIZE - 1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_sequential = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("顺序排序: %.3f 秒\n", time_sequential);
    printf("加速比: %.2fx\n", time_sequential / time_parallel);
    
    return 0;
}