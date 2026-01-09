#include <stdio.h>
#include <pthread.h>
#include <errno.h>

pthread_mutex_t mutex;

int main() {
    pthread_mutexattr_t attr;
    int ret;
    
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &attr);
    
    // 第一次加锁
    ret = pthread_mutex_lock(&mutex);
    if (ret == 0) {
        printf("第一次加锁成功\n");
    }
    
    // 重复加锁
    ret = pthread_mutex_lock(&mutex);
    if (ret == EDEADLK) {
        printf("重复加锁检测到错误\n");
    }
    
    pthread_mutex_unlock(&mutex);
    
    // 解锁未锁定的互斥锁
    ret = pthread_mutex_unlock(&mutex);
    if (ret == EPERM) {
        printf("解锁未锁定的互斥锁检测到错误\n");
    }
    
    pthread_mutex_destroy(&mutex);
    pthread_mutexattr_destroy(&attr);
    
    return 0;
}