#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


/* #define DYNAMIC_INIT */
#ifdef DYNAMIC_INIT
pthread_rwlock_t rwlock;
#else
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
#endif
int shared_data = 0;

void *reader(void *arg) {
    int id = (int)(long)arg;
    
    pthread_rwlock_rdlock(&rwlock);
    printf("读者 %d: 读取数据 %d\n", id, shared_data);
    sleep(1);
    printf("读者 %d: 完成读取\n", id);
    pthread_rwlock_unlock(&rwlock);
    
    return NULL;
}

void *writer(void *arg) {
    int id = (int)(long)arg;
    
    pthread_rwlock_wrlock(&rwlock);
    printf("写者 %d: 开始写入\n", id);
    shared_data++;
    sleep(2);
    printf("写者 %d: 完成写入，数据 = %d\n", id, shared_data);
    pthread_rwlock_unlock(&rwlock);
    
    return NULL;
}

int main() {
    pthread_t threads[5];
    
#ifdef DYNAMIC_INIT
    pthread_rwlock_init(&rwlock, NULL);
#endif
    
    // 创建读者和写者
    pthread_create(&threads[0], NULL, reader, (void *)1);
    pthread_create(&threads[1], NULL, reader, (void *)2);
    pthread_create(&threads[2], NULL, writer, (void *)1);
    pthread_create(&threads[3], NULL, reader, (void *)3);
    pthread_create(&threads[4], NULL, writer, (void *)2);
    
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_rwlock_destroy(&rwlock);
    
    return 0;
}
