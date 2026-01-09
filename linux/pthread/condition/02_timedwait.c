#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int condition = 0;

void *waiter(void *arg) {
    (void)arg;
    struct timespec ts;
    int ret;
    
    pthread_mutex_lock(&mutex);
    
    // 设置超时时间
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 5;  // 5秒超时
    
    while (!condition) {
        ret = pthread_cond_timedwait(&cond, &mutex, &ts);
        
        if (ret == ETIMEDOUT) {
            printf("等待超时\n");
            break;
        } else if (ret != 0) {
            printf("等待错误: %d\n", ret);
            break;
        }
    }
    
    if (condition) {
        printf("条件满足\n");
    }
    
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t tid;
    
    pthread_create(&tid, NULL, waiter, NULL);
    
    sleep(3);
    
    pthread_mutex_lock(&mutex);
    condition = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    
    pthread_join(tid, NULL);
    
    return 0;
}
