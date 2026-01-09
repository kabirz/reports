#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define DATA_SIZE 1000

int data[DATA_SIZE];
pthread_rwlock_t rwlock;

void *reader(void *arg) {
    int id = (int)(long)arg;
    int sum = 0;
    
    pthread_rwlock_rdlock(&rwlock);
    
    for (int i = 0; i < DATA_SIZE; i++) {
        sum += data[i];
    }
    
    printf("读者 %d: 总和 = %d\n", id, sum);
    
    pthread_rwlock_unlock(&rwlock);
    
    return NULL;
}

void *writer(void *arg) {
    int id = (int)(long)arg;
    
    pthread_rwlock_wrlock(&rwlock);
    
    for (int i = 0; i < DATA_SIZE; i++) {
        data[i] = i * id;
    }
    
    printf("写者 %d: 更新数据完成\n", id);
    
    pthread_rwlock_unlock(&rwlock);
    
    return NULL;
}

int main() {
    pthread_t threads[4];
    
    pthread_rwlock_init(&rwlock, NULL);
    
    // 初始化数据
    for (int i = 0; i < DATA_SIZE; i++) {
        data[i] = i;
    }
    
    // 创建读者和写者
    pthread_create(&threads[0], NULL, reader, (void *)1);
    pthread_create(&threads[1], NULL, reader, (void *)2);
    pthread_create(&threads[2], NULL, writer, (void *)1);
    pthread_create(&threads[3], NULL, reader, (void *)3);
    
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_rwlock_destroy(&rwlock);
    
    return 0;
}