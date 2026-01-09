#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int shared_data = 0;

// 好的做法：最小化临界区
void *good_critical_section(void *arg) {
    int id = (int)(long)arg;
    int local_data;
    
    // 只在必要时持有锁
    pthread_mutex_lock(&mutex);
    local_data = shared_data;
    pthread_mutex_unlock(&mutex);
    
    // 在临界区外执行耗时操作
    printf("线程 %d: 处理数据 %d\n", id, local_data);
    sleep(1);
    
    pthread_mutex_lock(&mutex);
    shared_data = local_data + 1;
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

// 避免的做法：临界区过大
void *bad_critical_section(void *arg) {
    int id = (int)(long)arg;
    
    // 临界区过大，包含耗时操作
    pthread_mutex_lock(&mutex);
    printf("线程 %d: 开始处理\n", id);
    sleep(1);  // 耗时操作在临界区内
    shared_data++;
    printf("线程 %d: 完成处理\n", id);
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    printf("=== 好的做法：最小化临界区 ===\n");
    pthread_create(&tid1, NULL, good_critical_section, (void *)1);
    pthread_create(&tid2, NULL, good_critical_section, (void *)2);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("最终数据: %d\n\n", shared_data);
    
    shared_data = 0;
    
    printf("=== 避免的做法：临界区过大 ===\n");
    pthread_create(&tid1, NULL, bad_critical_section, (void *)1);
    pthread_create(&tid2, NULL, bad_critical_section, (void *)2);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("最终数据: %d\n", shared_data);
    
    return 0;
}