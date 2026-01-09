#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define ITERATIONS 10000000

pthread_mutex_t mutex;
pthread_spinlock_t spinlock;
int counter_mutex = 0;
int counter_spin = 0;

void *increment_mutex(void *arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        counter_mutex++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *increment_spin(void *arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_spin_lock(&spinlock);
        counter_spin++;
        pthread_spin_unlock(&spinlock);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    struct timespec start, end;
    double time_mutex, time_spin;
    
    // 测试互斥锁
    pthread_mutex_init(&mutex, NULL);
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_create(&tid1, NULL, increment_mutex, NULL);
    pthread_create(&tid2, NULL, increment_mutex, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    time_mutex = (end.tv_sec - start.tv_sec) + 
                 (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("互斥锁: 计数器 = %d, 时间 = %.3f 秒\n", 
           counter_mutex, time_mutex);
    
    pthread_mutex_destroy(&mutex);
    
    // 测试自旋锁
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_create(&tid1, NULL, increment_spin, NULL);
    pthread_create(&tid2, NULL, increment_spin, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    time_spin = (end.tv_sec - start.tv_sec) + 
                (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("自旋锁: 计数器 = %d, 时间 = %.3f 秒\n", 
           counter_spin, time_spin);
    
    pthread_spin_destroy(&spinlock);
    
    return 0;
}