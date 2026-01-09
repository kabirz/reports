#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>

#define ITERATIONS 1000000
#define THREADS 4

// 测试数据结构
typedef struct {
    pthread_mutex_t mutex;
    pthread_spinlock_t spinlock;
    pthread_rwlock_t rwlock;
    sem_t semaphore;
    int counter;
} test_data_t;

// 互斥锁测试
void *test_mutex(void *arg) {
    test_data_t *data = (test_data_t *)arg;
    
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&data->mutex);
        data->counter++;
        pthread_mutex_unlock(&data->mutex);
    }
    
    return NULL;
}

// 自旋锁测试
void *test_spinlock(void *arg) {
    test_data_t *data = (test_data_t *)arg;
    
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_spin_lock(&data->spinlock);
        data->counter++;
        pthread_spin_unlock(&data->spinlock);
    }
    
    return NULL;
}

// 读写锁测试（写锁）
void *test_rwlock(void *arg) {
    test_data_t *data = (test_data_t *)arg;
    
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_rwlock_wrlock(&data->rwlock);
        data->counter++;
        pthread_rwlock_unlock(&data->rwlock);
    }
    
    return NULL;
}

// 信号量测试
void *test_semaphore(void *arg) {
    test_data_t *data = (test_data_t *)arg;
    
    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(&data->semaphore);
        data->counter++;
        sem_post(&data->semaphore);
    }
    
    return NULL;
}

// 性能测试函数
double run_test(void *(*test_func)(void *), test_data_t *data) {
    pthread_t threads[THREADS];
    struct timespec start, end;
    
    data->counter = 0;
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < THREADS; i++) {
        pthread_create(&threads[i], NULL, test_func, data);
    }
    
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    test_data_t data;
    double time_mutex, time_spinlock, time_rwlock, time_semaphore;
    
    // 初始化同步机制
    pthread_mutex_init(&data.mutex, NULL);
    pthread_spin_init(&data.spinlock, PTHREAD_PROCESS_PRIVATE);
    pthread_rwlock_init(&data.rwlock, NULL);
    sem_init(&data.semaphore, 0, 1);
    
    printf("性能测试: %d 线程，每线程 %d 次迭代\n", THREADS, ITERATIONS);
    printf("------------------------------------------------\n");
    
    // 测试互斥锁
    time_mutex = run_test(test_mutex, &data);
    printf("互斥锁:   计数器 = %d, 时间 = %.3f 秒\n", 
           data.counter, time_mutex);
    
    // 测试自旋锁
    time_spinlock = run_test(test_spinlock, &data);
    printf("自旋锁:   计数器 = %d, 时间 = %.3f 秒\n", 
           data.counter, time_spinlock);
    
    // 测试读写锁
    time_rwlock = run_test(test_rwlock, &data);
    printf("读写锁:   计数器 = %d, 时间 = %.3f 秒\n", 
           data.counter, time_rwlock);
    
    // 测试信号量
    time_semaphore = run_test(test_semaphore, &data);
    printf("信号量:   计数器 = %d, 时间 = %.3f 秒\n", 
           data.counter, time_semaphore);
    
    printf("------------------------------------------------\n");
    printf("相对性能:\n");
    printf("互斥锁:   1.00x (基准)\n");
    printf("自旋锁:   %.2fx\n", time_mutex / time_spinlock);
    printf("读写锁:   %.2fx\n", time_mutex / time_rwlock);
    printf("信号量:   %.2fx\n", time_mutex / time_semaphore);
    
    // 清理
    pthread_mutex_destroy(&data.mutex);
    pthread_spin_destroy(&data.spinlock);
    pthread_rwlock_destroy(&data.rwlock);
    sem_destroy(&data.semaphore);
    
    return 0;
}