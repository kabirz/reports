#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_POOL_SIZE 4
#define TASK_QUEUE_SIZE 10

typedef struct task {
    void (*function)(void *);
    void *arg;
    struct task *next;
} task_t;

typedef struct {
    pthread_t threads[THREAD_POOL_SIZE];
    task_t *task_queue;
    int task_count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int shutdown;
} thread_pool_t;

thread_pool_t pool;

void *worker(void *arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&pool.mutex);
        
        while (pool.task_count == 0 && !pool.shutdown) {
            pthread_cond_wait(&pool.cond, &pool.mutex);
        }
        
        if (pool.shutdown) {
            pthread_mutex_unlock(&pool.mutex);
            pthread_exit(NULL);
        }
        
        task_t *task = pool.task_queue;
        pool.task_queue = task->next;
        pool.task_count--;
        
        pthread_mutex_unlock(&pool.mutex);
        
        task->function(task->arg);
        free(task);
    }
    
    return NULL;
}

void task_function(void *arg) {
    int id = *(int *)arg;
    printf("任务 %d 执行中\n", id);
    sleep(1);
    printf("任务 %d 完成\n", id);
    free(arg);
}

void thread_pool_init() {
    pool.task_queue = NULL;
    pool.task_count = 0;
    pool.shutdown = 0;
    pthread_mutex_init(&pool.mutex, NULL);
    pthread_cond_init(&pool.cond, NULL);
    
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&pool.threads[i], NULL, worker, NULL);
    }
}

void thread_pool_submit(void (*function)(void *), void *arg) {
    task_t *task = malloc(sizeof(task_t));
    task->function = function;
    task->arg = arg;
    task->next = NULL;
    
    pthread_mutex_lock(&pool.mutex);
    
    if (pool.task_queue == NULL) {
        pool.task_queue = task;
    } else {
        task_t *current = pool.task_queue;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = task;
    }
    
    pool.task_count++;
    pthread_cond_signal(&pool.cond);
    
    pthread_mutex_unlock(&pool.mutex);
}

void thread_pool_shutdown() {
    pthread_mutex_lock(&pool.mutex);
    pool.shutdown = 1;
    pthread_cond_broadcast(&pool.cond);
    pthread_mutex_unlock(&pool.mutex);
    
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_join(pool.threads[i], NULL);
    }
    
    pthread_mutex_destroy(&pool.mutex);
    pthread_cond_destroy(&pool.cond);
}

int main() {
    thread_pool_init();
    
    for (int i = 0; i < 10; i++) {
        int *arg = malloc(sizeof(int));
        *arg = i;
        thread_pool_submit(task_function, arg);
    }
    
    sleep(5);
    thread_pool_shutdown();
    
    return 0;
}