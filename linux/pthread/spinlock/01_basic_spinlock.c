#include <stdio.h>
#include <pthread.h>

pthread_spinlock_t spinlock;
int counter = 0;

void *increment(void *arg) {
    (void)arg;
    for (int i = 0; i < 100000; i++) {
        pthread_spin_lock(&spinlock);
        counter++;
        pthread_spin_unlock(&spinlock);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
    
    pthread_create(&tid1, NULL, increment, NULL);
    pthread_create(&tid2, NULL, increment, NULL);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    printf("计数器值: %d\n", counter);
    
    pthread_spin_destroy(&spinlock);
    
    return 0;
}