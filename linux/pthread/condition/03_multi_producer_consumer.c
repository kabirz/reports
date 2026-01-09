#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define PRODUCER_COUNT 3
#define CONSUMER_COUNT 3

int buffer[BUFFER_SIZE];
int count = 0;
int in = 0, out = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    int id = (int)(long)arg;
    int item = 0;
    
    while (1) {
        pthread_mutex_lock(&mutex);
        
        while (count == BUFFER_SIZE) {
            printf("生产者 %d: 缓冲区满，等待\n", id);
            pthread_cond_wait(&not_full, &mutex);
        }
        
        buffer[in] = item;
        printf("生产者 %d: 生产 %d\n", id, item);
        in = (in + 1) % BUFFER_SIZE;
        count++;
        item++;
        
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = (int)(long)arg;
    
    while (1) {
        pthread_mutex_lock(&mutex);
        
        while (count == 0) {
            printf("消费者 %d: 缓冲区空，等待\n", id);
            pthread_cond_wait(&not_empty, &mutex);
        }
        
        int item = buffer[out];
        printf("消费者 %d: 消费 %d\n", id, item);
        out = (out + 1) % BUFFER_SIZE;
        count--;
        
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t producers[PRODUCER_COUNT];
    pthread_t consumers[CONSUMER_COUNT];
    
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_create(&producers[i], NULL, producer, (void *)(long)i);
    }
    
    for (int i = 0; i < CONSUMER_COUNT; i++) {
        pthread_create(&consumers[i], NULL, consumer, (void *)(long)i);
    }
    
    // 让程序运行一段时间
    sleep(30);
    
    return 0;
}