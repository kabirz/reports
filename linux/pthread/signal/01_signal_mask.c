#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

sigset_t mask;

void *signal_thread(void *arg) {
    int sig;
    
    (void)arg;
    printf("信号线程: 等待信号...\n");
    
    while (1) {
        if (sigwait(&mask, &sig) == 0) {
            printf("信号线程: 收到信号 %d\n", sig);
        }
    }
    
    return NULL;
}

void *worker_thread(void *arg) {
    int id = (int)(long)arg;
    
    printf("工作线程 %d: 开始工作\n", id);
    sleep(5);
    printf("工作线程 %d: 完成工作\n", id);
    
    return NULL;
}

int main() {
    pthread_t signal_tid, worker_tid;
    
    // 设置信号掩码
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    
    // 创建信号处理线程
    pthread_create(&signal_tid, NULL, signal_thread, NULL);
    
    // 创建工作线程
    pthread_create(&worker_tid, NULL, worker_thread, (void *)1);
    
    sleep(2);
    
    // 发送信号
    printf("主线程: 发送 SIGUSR1\n");
    pthread_kill(signal_tid, SIGUSR1);
    
    sleep(1);
    
    printf("主线程: 发送 SIGUSR2\n");
    pthread_kill(signal_tid, SIGUSR2);
    
    pthread_join(worker_tid, NULL);
    pthread_cancel(signal_tid);
    pthread_join(signal_tid, NULL);
    
    return 0;
}