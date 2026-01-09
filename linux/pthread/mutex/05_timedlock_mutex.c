#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t mutex;

void *thread_func(void *arg) {
    int id = (int)(long)arg;
    struct timespec ts;
    
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 3;  // 3秒超时
    
    if (pthread_mutex_timedlock(&mutex, &ts) == 0) {
        printf("线程 %d 获得锁\n", id);
        sleep(2);
        pthread_mutex_unlock(&mutex);
    } else {
        printf("线程 %d 获取锁超时\n", id);
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
