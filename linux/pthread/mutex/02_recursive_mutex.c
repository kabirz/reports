#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;

void recursive_function(int depth) {
    pthread_mutex_lock(&mutex);
    printf("递归深度: %d\n", depth);
    
    if (depth > 0) {
        recursive_function(depth - 1);
    }
    
    pthread_mutex_unlock(&mutex);
}

int main() {
    pthread_mutexattr_t attr;
    
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex, &attr);
    
    recursive_function(15);
    
    pthread_mutex_destroy(&mutex);
    pthread_mutexattr_destroy(&attr);
    
    return 0;
}
