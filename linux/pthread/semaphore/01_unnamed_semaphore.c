#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t empty_slots;
sem_t full_slots;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer(void *arg) {
    int id = (int)(long)arg;
    int item = 0;
    
    while (1) {
        sem_wait(&empty_slots);
        
        pthread_mutex_lock(&mutex);
        buffer[in] = item;
        printf("生产者 %d: 生产 %d\n", id, item);
        in = (in + 1) % BUFFER_SIZE;
        item++;
        pthread_mutex_unlock(&mutex);
        
        sem_post(&full_slots);
        
        sleep(1);
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = (int)(long)arg;
    
    while (1) {
        sem_wait(&full_slots);
        
        pthread_mutex_lock(&mutex);
        int item = buffer[out];
        printf("消费者 %d: 消费 %d\n", id, item);
        out = (out + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);
        
        sem_post(&empty_slots);
        
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;
    
    sem_init(&empty_slots, 0, BUFFER_SIZE);
    sem_init(&full_slots, 0, 0);
    
    pthread_create(&producer_thread, NULL, producer, (void *)1);
    pthread_create(&consumer_thread, NULL, consumer, (void *)1);
    
    sleep(30);
    
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    
    return 0;
}