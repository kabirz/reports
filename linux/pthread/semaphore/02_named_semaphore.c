#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>

#define SEM_NAME "/mysemaphore"

int main() {
    sem_t *sem;
    pid_t pid;
    
    // 创建命名信号量
    sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }
    
    pid = fork();
    
    if (pid == 0) {
        // 子进程
        sem_wait(sem);
        printf("子进程: 进入临界区\n");
        sleep(2);
        printf("子进程: 离开临界区\n");
        sem_post(sem);
        sem_close(sem);
    } else {
        // 父进程
        sleep(1);
        sem_wait(sem);
        printf("父进程: 进入临界区\n");
        sleep(2);
        printf("父进程: 离开临界区\n");
        sem_post(sem);
        wait(NULL);
        
        sem_close(sem);
        sem_unlink(SEM_NAME);
    }
    
    return 0;
}