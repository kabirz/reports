#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 场景1: 简单的互斥访问
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int shared_counter = 0;

void *simple_mutex_example(void *arg) {
    (void)arg;
    pthread_mutex_lock(&mutex);
    shared_counter++;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

// 场景2: 读多写少
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int shared_data = 0;

void *reader_example(void *arg) {
    (void)arg;
    pthread_rwlock_rdlock(&rwlock);
    printf("读取数据: %d\n", shared_data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void *writer_example(void *arg) {
    (void)arg;
    pthread_rwlock_wrlock(&rwlock);
    shared_data++;
    printf("写入数据: %d\n", shared_data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

// 场景3: 等待条件
pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int condition = 0;

void *waiter_example(void *arg) {
    (void)arg;
    pthread_mutex_lock(&cond_mutex);
    while (!condition) {
        pthread_cond_wait(&cond, &cond_mutex);
    }
    printf("条件满足\n");
    pthread_mutex_unlock(&cond_mutex);
    return NULL;
}

void *signaler_example(void *arg) {
    (void)arg;
    sleep(1);
    pthread_mutex_lock(&cond_mutex);
    condition = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&cond_mutex);
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    printf("=== 场景1: 简单的互斥访问 ===\n");
    pthread_create(&tid1, NULL, simple_mutex_example, NULL);
    pthread_create(&tid2, NULL, simple_mutex_example, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("计数器值: %d\n\n", shared_counter);
    
    printf("=== 场景2: 读多写少 ===\n");
    pthread_create(&tid1, NULL, reader_example, NULL);
    pthread_create(&tid2, NULL, writer_example, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("\n");
    
    printf("=== 场景3: 等待条件 ===\n");
    pthread_create(&tid1, NULL, waiter_example, NULL);
    pthread_create(&tid2, NULL, signaler_example, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("\n");
    
    return 0;
}