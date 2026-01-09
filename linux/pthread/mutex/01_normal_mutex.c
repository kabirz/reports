#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;
int counter = 0;

void *increment(void *arg) {
    (void)arg;
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2, tid3;
    pthread_mutexattr_t attr;
    
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&mutex, &attr);
    
    pthread_create(&tid1, NULL, increment, NULL);
    pthread_create(&tid2, NULL, increment, NULL);
    pthread_create(&tid3, NULL, increment, NULL);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    
    printf("计数器值: %d\n", counter);
    
    pthread_mutex_destroy(&mutex);
    pthread_mutexattr_destroy(&attr);
    
    return 0;
}
