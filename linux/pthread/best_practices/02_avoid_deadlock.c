#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

// 好的做法：按固定顺序获取锁
void *good_lock_order(void *arg) {
    int id = (int)(long)arg;
    
    // 按固定顺序获取锁
    pthread_mutex_lock(&mutex1);
    printf("线程 %d: 获取 mutex1\n", id);
    sleep(1);
    
    pthread_mutex_lock(&mutex2);
    printf("线程 %d: 获取 mutex2\n", id);
    sleep(1);
    
    printf("线程 %d: 释放锁\n", id);
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    
    return NULL;
}

// 避免的做法：可能导致死锁
void *bad_lock_order(void *arg) {
    int id = (int)(long)arg;
    
    if (id == 1) {
        pthread_mutex_lock(&mutex1);
        printf("线程 %d: 获取 mutex1\n", id);
        sleep(1);
        
        pthread_mutex_lock(&mutex2);
        printf("线程 %d: 获取 mutex2\n", id);
        sleep(1);
        
        printf("线程 %d: 释放锁\n", id);
        pthread_mutex_unlock(&mutex2);
        pthread_mutex_unlock(&mutex1);
    } else {
        pthread_mutex_lock(&mutex2);
        printf("线程 %d: 获取 mutex2\n", id);
        sleep(1);
        
        pthread_mutex_lock(&mutex1);
        printf("线程 %d: 获取 mutex1\n", id);
        sleep(1);
        
        printf("线程 %d: 释放锁\n", id);
        pthread_mutex_unlock(&mutex1);
        pthread_mutex_unlock(&mutex2);
    }
    
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    printf("=== 好的做法：按固定顺序获取锁 ===\n");
    pthread_create(&tid1, NULL, good_lock_order, (void *)1);
    pthread_create(&tid2, NULL, good_lock_order, (void *)2);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("\n");
    
    printf("=== 避免的做法：可能导致死锁 ===\n");
    printf("（此示例可能死锁，需要手动终止）\n");
    pthread_create(&tid1, NULL, bad_lock_order, (void *)1);
    pthread_create(&tid2, NULL, bad_lock_order, (void *)2);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    return 0;
}