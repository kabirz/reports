#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int count = 0;
int in = 0, out = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    (void)arg;
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);
        
        // 等待缓冲区不满
        while (count == BUFFER_SIZE) {
            printf("缓冲区满，生产者等待\n");
            pthread_cond_wait(&not_full, &mutex);
        }
        
        // 生产数据
        buffer[in] = i;
        printf("生产者生产: %d\n", i);
        in = (in + 1) % BUFFER_SIZE;
        count++;
        
        // 通知消费者
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }
    return NULL;
}

void *consumer(void *arg) {
    (void)arg;
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);
        
        // 等待缓冲区不空
        while (count == 0) {
            printf("缓冲区空，消费者等待\n");
            pthread_cond_wait(&not_empty, &mutex);
        }
        
        // 消费数据
        int item = buffer[out];
        printf("消费者消费: %d\n", item);
        out = (out + 1) % BUFFER_SIZE;
        count--;
        
        // 通知生产者
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;
    
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);
    
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    
    return 0;
}
