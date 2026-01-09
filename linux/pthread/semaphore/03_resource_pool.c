#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define RESOURCE_COUNT 3
#define THREAD_COUNT 5

sem_t resource_sem;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int resources[RESOURCE_COUNT];

void *worker(void *arg) {
    int id = (int)(long)arg;
    int resource_id;
    
    // 等待可用资源
    sem_wait(&resource_sem);
    
    // 分配资源
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        if (resources[i] == 0) {
            resources[i] = id;
            resource_id = i;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    
    printf("线程 %d: 使用资源 %d\n", id, resource_id);
    sleep(2);
    
    // 释放资源
    pthread_mutex_lock(&mutex);
    resources[resource_id] = 0;
    pthread_mutex_unlock(&mutex);
    
    printf("线程 %d: 释放资源 %d\n", id, resource_id);
    sem_post(&resource_sem);
    
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    
    sem_init(&resource_sem, 0, RESOURCE_COUNT);
    
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        resources[i] = 0;
    }
    
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, worker, (void *)(long)i);
    }
    
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
    
    sem_destroy(&resource_sem);
    
    return 0;
}