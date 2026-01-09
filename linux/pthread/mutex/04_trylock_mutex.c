#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex;

void *thread_func(void *arg) {
    int id = (int)(long)arg;
    
    while (true) {
        if (pthread_mutex_trylock(&mutex) == 0) {
            printf("线程 %d 获得锁\n", id);
            sleep(2);
            printf("线程 %d 释放锁\n", id);
            pthread_mutex_unlock(&mutex);
            break;
        } else {
            printf("线程 %d 未能获得锁\n", id);
            sleep(1);
        }
    }
    
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    pthread_mutex_init(&mutex, NULL);
    
    pthread_create(&tid1, NULL, thread_func, (void *)1);
    pthread_create(&tid2, NULL, thread_func, (void *)2);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
