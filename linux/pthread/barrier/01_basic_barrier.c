#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_barrier_t barrier;

void *worker(void *arg) {
    int id = (int)(long)arg;
    
    printf("线程 %d: 开始工作\n", id);
    sleep(id);  // 模拟不同工作时间
    
    printf("线程 %d: 完成工作，等待其他线程\n", id);
    pthread_barrier_wait(&barrier);
    
    printf("线程 %d: 继续执行\n", id);
    
    return NULL;
}

int main() {
    pthread_t threads[3];
    
    // 初始化屏障，等待3个线程
    pthread_barrier_init(&barrier, NULL, 3);
    
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, worker, (void *)(long)i);
    }
    
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_barrier_destroy(&barrier);
    
    return 0;
}