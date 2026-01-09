#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

timer_t timerid;
volatile sig_atomic_t timer_flag = 0;

void timer_handler(int sig) {
    (void)sig;
    printf("定时器信号: 收到 SIGRTMIN\n");
    timer_flag = 1;
}

void *timer_thread(void *arg) {
    struct sigevent sev;
    struct itimerspec its;
    sigset_t mask;
    int sig;
    
    (void)arg;
    // 设置信号掩码
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    
    // 设置定时器
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timerid;
    
    timer_create(CLOCK_REALTIME, &sev, &timerid);
    
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 1;
    its.it_interval.tv_nsec = 0;
    
    timer_settime(timerid, 0, &its, NULL);
    
    printf("定时器线程: 等待定时器信号...\n");
    
    while (1) {
        if (sigwait(&mask, &sig) == 0) {
            printf("定时器线程: 定时器触发\n");
            timer_flag = 1;
        }
    }
    
    return NULL;
}

int main() {
    pthread_t timer_tid;
    
    pthread_create(&timer_tid, NULL, timer_thread, NULL);
    
    sleep(5);
    
    pthread_cancel(timer_tid);
    pthread_join(timer_tid, NULL);
    
    timer_delete(timerid);
    
    return 0;
}