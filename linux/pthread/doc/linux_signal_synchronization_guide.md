# Linux 应用信号同步方法详细指南

## 目录
1. [信号基础概念](#信号基础概念)
2. [信号处理机制](#信号处理机制)
3. [信号处理函数和API](#信号处理函数和api)
4. [信号量同步](#信号量同步)
5. [互斥锁同步](#互斥锁同步)
6. [条件变量同步](#条件变量同步)
7. [进程间信号同步](#进程间信号同步)
8. [线程间信号同步](#线程间信号同步)
9. [高级同步技术](#高级同步技术)
10. [实际应用示例](#实际应用示例)
11. [最佳实践](#最佳实践)
12. [故障排查](#故障排查)

---

## 信号基础概念

### 什么是信号
信号是Linux系统中一种异步通信机制，用于通知进程发生了某个事件。信号可以由内核、其他进程或进程自身发送，用于进程控制、异常处理、进程间通信等。

### 信号的特点
- **异步性**: 信号可以在任何时候发送，不依赖于进程的执行状态
- **优先级**: 信号有优先级，某些信号会中断其他信号的处理
- **不可靠性**: 信号可能会丢失，特别是在信号队列满时
- **轻量级**: 信号传递的开销很小

### 标准信号列表

| 信号编号 | 信号名称 | 默认行为 | 说明 |
|---------|---------|---------|------|
| 1 | SIGHUP | 终止 | 终端挂起或控制进程终止 |
| 2 | SIGINT | 终止 | 键盘中断（Ctrl+C） |
| 3 | SIGQUIT | 终止+core | 键盘退出（Ctrl+\） |
| 4 | SIGILL | 终止+core | 非法指令 |
| 5 | SIGTRAP | 终止+core | 断点陷阱 |
| 6 | SIGABRT | 终止+core | abort()调用 |
| 7 | SIGBUS | 终止+core | 总线错误 |
| 8 | SIGFPE | 终止+core | 浮点异常 |
| 9 | SIGKILL | 终止 | 强制终止（不可捕获） |
| 10 | SIGUSR1 | 终止 | 用户定义信号1 |
| 11 | SIGSEGV | 终止+core | 段错误 |
| 12 | SIGUSR2 | 终止 | 用户定义信号2 |
| 13 | SIGPIPE | 终止 | 管道破裂 |
| 14 | SIGALRM | 终止 | 定时器到期 |
| 15 | SIGTERM | 终止 | 终止信号 |
| 17 | SIGCHLD | 忽略 | 子进程状态改变 |
| 19 | SIGSTOP | 停止 | 进程停止（不可捕获） |
| 20 | SIGTSTP | 停止 | 终端停止（Ctrl+Z） |
| 21 | SIGTTIN | 停止 | 后台进程读终端 |
| 22 | SIGTTOU | 停止 | 后台进程写终端 |

### 实时信号
Linux还支持实时信号（SIGRTMIN到SIGRTMAX），具有以下特点：
- 支持排队，不会丢失
- 可以携带数据
- 有优先级
- 数量较多（通常32个）

---

## 信号处理机制

### 信号的生命周期
```
信号产生 → 信号传递 → 信号处理 → 信号返回
```

### 信号处理方式
进程对信号有三种处理方式：
1. **默认处理**: 执行系统默认的动作
2. **忽略处理**: 忽略该信号
3. **自定义处理**: 执行用户定义的处理函数

### 信号掩码
信号掩码（Signal Mask）是一组被阻塞的信号，这些信号在掩码中时不会被传递给进程。

### 信号集操作
```c
#include <signal.h>

// 初始化信号集
int sigemptyset(sigset_t *set);           // 清空信号集
int sigfillset(sigset_t *set);            // 填充所有信号
int sigaddset(sigset_t *set, int signo);  // 添加信号
int sigdelset(sigset_t *set, int signo);  // 删除信号
int sigismember(const sigset_t *set, int signo); // 检查信号是否在集合中
```

### 信号处理流程
```
1. 信号产生
   ↓
2. 检查信号掩码
   ↓
3. 如果信号被阻塞，加入待处理队列
   ↓
4. 如果信号未被阻塞，传递给进程
   ↓
5. 执行信号处理函数
   ↓
6. 从信号处理函数返回
```

---

## 信号处理函数和API

### signal() 函数
```c
#include <signal.h>

typedef void (*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler);
```

**参数说明:**
- `signum`: 信号编号
- `handler`: 信号处理函数，可以是：
  - `SIG_IGN`: 忽略信号
  - `SIG_DFL`: 使用默认处理
  - 自定义函数指针

**示例:**
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int signum) {
    printf("收到 SIGINT 信号 (Ctrl+C)\n");
}

int main() {
    // 注册 SIGINT 处理函数
    signal(SIGINT, sigint_handler);
    
    printf("进程 PID: %d\n", getpid());
    printf("按 Ctrl+C 测试信号处理\n");
    
    while(1) {
        sleep(1);
    }
    
    return 0;
}
```

### sigaction() 函数
```c
#include <signal.h>

struct sigaction {
    void (*sa_handler)(int);           // 信号处理函数
    void (*sa_sigaction)(int, siginfo_t *, void *); // 扩展处理函数
    sigset_t sa_mask;                  // 信号掩码
    int sa_flags;                      // 标志位
    void (*sa_restorer)(void);         // 已废弃
};

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```

**标志位 (sa_flags):**
- `SA_SIGINFO`: 使用扩展处理函数
- `SA_RESTART`: 被中断的系统调用自动重启
- `SA_NOCLDSTOP`: 子进程停止时不产生 SIGCHLD
- `SA_NOCLDWAIT`: 子进程终止时不变成僵尸进程
- `SA_NODEFER`: 在处理函数中不阻塞该信号

**示例:**
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sigaction_handler(int signum, siginfo_t *info, void *context) {
    printf("收到信号: %d\n", signum);
    printf("发送进程 PID: %d\n", info->si_pid);
    printf("发送进程 UID: %d\n", info->si_uid);
    printf("信号值: %d\n", info->si_value.sival_int);
}

int main() {
    struct sigaction sa;
    
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = sigaction_handler;
    sa.sa_flags = SA_SIGINFO;
    
    // 注册信号处理函数
    sigaction(SIGUSR1, &sa, NULL);
    
    printf("进程 PID: %d\n", getpid());
    printf("发送信号: kill -USR1 %d\n", getpid());
    
    while(1) {
        sleep(1);
    }
    
    return 0;
}
```

### 发送信号

#### kill() 函数
```c
#include <sys/types.h>
#include <signal.h>

int kill(pid_t pid, int sig);
```

**参数说明:**
- `pid > 0`: 发送给指定进程
- `pid = 0`: 发送给同进程组的所有进程
- `pid = -1`: 发送给所有有权限的进程
- `pid < -1`: 发送给进程组为 |pid| 的所有进程

**示例:**
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子进程
        printf("子进程 PID: %d\n", getpid());
        while(1) {
            sleep(1);
        }
    } else {
        // 父进程
        sleep(2);
        printf("父进程发送 SIGTERM 给子进程\n");
        kill(pid, SIGTERM);
        wait(NULL);
        printf("子进程已终止\n");
    }
    
    return 0;
}
```

#### raise() 函数
```c
#include <signal.h>

int raise(int sig);
```

**示例:**
```c
#include <stdio.h>
#include <signal.h>

void handler(int signum) {
    printf("收到信号: %d\n", signum);
}

int main() {
    signal(SIGUSR1, handler);
    
    printf("发送信号给自己\n");
    raise(SIGUSR1);
    
    return 0;
}
```

#### sigqueue() 函数
```c
#include <signal.h>

int sigqueue(pid_t pid, int sig, const union sigval value);
```

**示例:**
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

void handler(int signum, siginfo_t *info, void *context) {
    printf("收到信号: %d\n", signum);
    printf("携带的数据: %d\n", info->si_value.sival_int);
}

int main() {
    struct sigaction sa;
    union sigval value;
    
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    
    sigaction(SIGUSR1, &sa, NULL);
    
    value.sival_int = 42;
    printf("发送信号并携带数据\n");
    sigqueue(getpid(), SIGUSR1, value);
    
    return 0;
}
```

### 信号掩码操作

#### sigprocmask() 函数
```c
#include <signal.h>

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
```

**参数说明:**
- `how`: 操作方式
  - `SIG_BLOCK`: 添加信号到掩码
  - `SIG_UNBLOCK`: 从掩码中移除信号
  - `SIG_SETMASK`: 设置新的掩码

**示例:**
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
    printf("收到信号: %d\n", signum);
}

int main() {
    sigset_t mask, oldmask;
    
    signal(SIGUSR1, handler);
    
    // 阻塞 SIGUSR1
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    
    printf("SIGUSR1 已阻塞\n");
    kill(getpid(), SIGUSR1);  // 信号被阻塞，不会立即处理
    
    sleep(2);
    
    // 解除阻塞
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    printf("SIGUSR1 已解除阻塞\n");
    
    return 0;
}
```

#### sigpending() 函数
```c
#include <signal.h>

int sigpending(sigset_t *set);
```

**示例:**
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
    printf("处理信号: %d\n", signum);
}

int main() {
    sigset_t mask, pending;
    
    signal(SIGUSR1, handler);
    
    // 阻塞 SIGUSR1
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    
    // 发送信号
    kill(getpid(), SIGUSR1);
    
    // 检查待处理信号
    sigpending(&pending);
    if (sigismember(&pending, SIGUSR1)) {
        printf("SIGUSR1 在待处理队列中\n");
    }
    
    // 解除阻塞
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    
    return 0;
}
```

---

## 信号量同步

### 信号量基础
信号量（Semaphore）是一种同步机制，用于控制多个进程或线程对共享资源的访问。

### System V 信号量

#### 创建和获取信号量
```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semget(key_t key, int nsems, int semflg);
```

**参数说明:**
- `key`: 信号量键值
- `nsems`: 信号量数量
- `semflg`: 标志位（IPC_CREAT, IPC_EXCL, 权限）

**示例:**
```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

int main() {
    key_t key;
    int semid;
    
    // 生成键值
    key = ftok("/tmp", 'A');
    if (key == -1) {
        perror("ftok");
        return 1;
    }
    
    // 创建信号量
    semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        return 1;
    }
    
    printf("信号量 ID: %d\n", semid);
    
    return 0;
}
```

#### 操作信号量
```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semop(int semid, struct sembuf *sops, unsigned nsops);
```

**结构体定义:**
```c
struct sembuf {
    unsigned short sem_num;  // 信号量索引
    short sem_op;            // 操作值
    short sem_flg;           // 标志位
};
```

**示例:**
```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

// P 操作（等待）
void P(int semid) {
    struct sembuf op = {0, -1, 0};
    semop(semid, &op, 1);
}

// V 操作（释放）
void V(int semid) {
    struct sembuf op = {0, 1, 0};
    semop(semid, &op, 1);
}

int main() {
    key_t key;
    int semid;
    
    key = ftok("/tmp", 'A');
    semid = semget(key, 1, IPC_CREAT | 0666);
    
    // 初始化信号量
    semctl(semid, 0, SETVAL, 1);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子进程
        for (int i = 0; i < 5; i++) {
            P(semid);
            printf("子进程: %d\n", i);
            sleep(1);
            V(semid);
        }
    } else {
        // 父进程
        for (int i = 0; i < 5; i++) {
            P(semid);
            printf("父进程: %d\n", i);
            sleep(1);
            V(semid);
        }
        wait(NULL);
        
        // 删除信号量
        semctl(semid, 0, IPC_RMID);
    }
    
    return 0;
}
```

#### 控制信号量
```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semctl(int semid, int semnum, int cmd, ...);
```

**常用命令:**
- `GETVAL`: 获取信号量值
- `SETVAL`: 设置信号量值
- `IPC_RMID`: 删除信号量

**示例:**
```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main() {
    key_t key;
    int semid;
    
    key = ftok("/tmp", 'A');
    semid = semget(key, 1, IPC_CREAT | 0666);
    
    // 设置信号量初始值
    semctl(semid, 0, SETVAL, 1);
    
    // 获取信号量值
    int value = semctl(semid, 0, GETVAL);
    printf("信号量值: %d\n", value);
    
    // 删除信号量
    semctl(semid, 0, IPC_RMID);
    
    return 0;
}
```

### POSIX 信号量

#### 命名信号量
```c
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

sem_t *sem_open(const char *name, int oflag, ...);
int sem_close(sem_t *sem);
int sem_unlink(const char *name);
```

**示例:**
```c
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>

int main() {
    sem_t *sem;
    
    // 创建命名信号量
    sem = sem_open("/mysem", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }
    
    // 等待信号量
    sem_wait(sem);
    printf("进入临界区\n");
    sleep(2);
    printf("离开临界区\n");
    // 释放信号量
    sem_post(sem);
    
    // 关闭信号量
    sem_close(sem);
    
    // 删除信号量
    sem_unlink("/mysem");
    
    return 0;
}
```

#### 未命名信号量
```c
#include <semaphore.h>

int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
```

**示例:**
```c
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

sem_t sem;

void *thread_func(void *arg) {
    for (int i = 0; i < 5; i++) {
        sem_wait(&sem);
        printf("线程 %ld: %d\n", (long)arg, i);
        sleep(1);
        sem_post(&sem);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    // 初始化信号量
    sem_init(&sem, 0, 1);
    
    // 创建线程
    pthread_create(&tid1, NULL, thread_func, (void *)1);
    pthread_create(&tid2, NULL, thread_func, (void *)2);
    
    // 等待线程结束
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    // 销毁信号量
    sem_destroy(&sem);
    
    return 0;
}
```

#### 信号量操作
```c
#include <semaphore.h>

int sem_wait(sem_t *sem);      // 等待（P操作）
int sem_trywait(sem_t *sem);   // 尝试等待
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout); // 超时等待
int sem_post(sem_t *sem);      // 释放（V操作）
int sem_getvalue(sem_t *sem, int *sval); // 获取值
```

**示例:**
```c
#include <stdio.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

int main() {
    sem_t sem;
    struct timespec ts;
    
    sem_init(&sem, 0, 0);
    
    // 超时等待
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 5;  // 5秒超时
    
    if (sem_timedwait(&sem, &ts) == 0) {
        printf("获取信号量成功\n");
    } else {
        printf("获取信号量超时\n");
    }
    
    sem_destroy(&sem);
    
    return 0;
}
```

---

## 互斥锁同步

### 互斥锁基础
互斥锁（Mutex）是一种用于保护共享资源的同步机制，确保同一时间只有一个线程可以访问共享资源。

### POSIX 互斥锁

#### 初始化和销毁
```c
#include <pthread.h>

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```

**示例:**
```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;

int main() {
    // 初始化互斥锁
    pthread_mutex_init(&mutex, NULL);
    
    // 使用互斥锁
    pthread_mutex_lock(&mutex);
    printf("临界区\n");
    pthread_mutex_unlock(&mutex);
    
    // 销毁互斥锁
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
```

#### 加锁和解锁
```c
#include <pthread.h>

int pthread_mutex_lock(pthread_mutex_t *mutex);      // 加锁
int pthread_mutex_trylock(pthread_mutex_t *mutex);   // 尝试加锁
int pthread_mutex_unlock(pthread_mutex_t *mutex);    // 解锁
```

**示例:**
```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;
int counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    pthread_mutex_init(&mutex, NULL);
    
    pthread_create(&tid1, NULL, increment, NULL);
    pthread_create(&tid2, NULL, increment, NULL);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    printf("计数器值: %d\n", counter);
    
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
```

#### 互斥锁属性
```c
#include <pthread.h>

int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
```

**互斥锁类型:**
- `PTHREAD_MUTEX_NORMAL`: 普通锁
- `PTHREAD_MUTEX_RECURSIVE`: 递归锁
- `PTHREAD_MUTEX_ERRORCHECK`: 错误检查锁
- `PTHREAD_MUTEX_DEFAULT`: 默认锁

**示例:**
```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_mutexattr_t attr;

int main() {
    // 初始化属性
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    
    // 初始化互斥锁
    pthread_mutex_init(&mutex, &attr);
    
    // 使用递归锁
    pthread_mutex_lock(&mutex);
    printf("第一次加锁\n");
    pthread_mutex_lock(&mutex);
    printf("第二次加锁\n");
    pthread_mutex_unlock(&mutex);
    pthread_mutex_unlock(&mutex);
    
    // 清理
    pthread_mutex_destroy(&mutex);
    pthread_mutexattr_destroy(&attr);
    
    return 0;
}
```

### 静态初始化
```c
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
```

**示例:**
```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    pthread_create(&tid1, NULL, increment, NULL);
    pthread_create(&tid2, NULL, increment, NULL);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    printf("计数器值: %d\n", counter);
    
    return 0;
}
```

---

## 条件变量同步

### 条件变量基础
条件变量（Condition Variable）是一种同步机制，用于线程间等待某个条件成立。

### 基本操作

#### 初始化和销毁
```c
#include <pthread.h>

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
```

#### 等待和通知
```c
#include <pthread.h>

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
int pthread_cond_signal(pthread_cond_t *cond);  // 唤醒一个等待线程
int pthread_cond_broadcast(pthread_cond_t *cond); // 唤醒所有等待线程
```

### 生产者-消费者模型
```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int count = 0;
int in = 0, out = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);
        
        while (count == BUFFER_SIZE) {
            printf("缓冲区满，生产者等待\n");
            pthread_cond_wait(&not_full, &mutex);
        }
        
        buffer[in] = i;
        printf("生产者生产: %d\n", i);
        in = (in + 1) % BUFFER_SIZE;
        count++;
        
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);
        
        while (count == 0) {
            printf("缓冲区空，消费者等待\n");
            pthread_cond_wait(&not_empty, &mutex);
        }
        
        int item = buffer[out];
        printf("消费者消费: %d\n", item);
        out = (out + 1) % BUFFER_SIZE;
        count--;
        
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;
    
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);
    
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    
    return 0;
}
```

### 超时等待
```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int condition = 0;

void *waiter(void *arg) {
    struct timespec ts;
    
    pthread_mutex_lock(&mutex);
    
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 5;  // 5秒超时
    
    while (!condition) {
        if (pthread_cond_timedwait(&cond, &mutex, &ts) == ETIMEDOUT) {
            printf("等待超时\n");
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
```

---

## 进程间信号同步

### 进程间信号发送
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

void handler(int signum) {
    printf("进程 %d 收到信号: %d\n", getpid(), signum);
}

int main() {
    pid_t pid;
    
    signal(SIGUSR1, handler);
    
    pid = fork();
    
    if (pid == 0) {
        // 子进程
        printf("子进程 PID: %d\n", getpid());
        printf("子进程等待信号...\n");
        pause();  // 等待信号
        printf("子进程继续执行\n");
    } else {
        // 父进程
        sleep(2);
        printf("父进程发送信号给子进程\n");
        kill(pid, SIGUSR1);
        wait(NULL);
    }
    
    return 0;
}
```

### 进程间信号量同步
```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

int semid;

void P(int semid) {
    struct sembuf op = {0, -1, 0};
    semop(semid, &op, 1);
}

void V(int semid) {
    struct sembuf op = {0, 1, 0};
    semop(semid, &op, 1);
}

void child_process() {
    for (int i = 0; i < 5; i++) {
        P(semid);
        printf("子进程: %d\n", i);
        sleep(1);
        V(semid);
    }
}

void parent_process() {
    for (int i = 0; i < 5; i++) {
        P(semid);
        printf("父进程: %d\n", i);
        sleep(1);
        V(semid);
    }
}

int main() {
    key_t key;
    pid_t pid;
    
    key = ftok("/tmp", 'A');
    semid = semget(key, 1, IPC_CREAT | 0666);
    semctl(semid, 0, SETVAL, 1);
    
    pid = fork();
    
    if (pid == 0) {
        child_process();
    } else {
        parent_process();
        wait(NULL);
        semctl(semid, 0, IPC_RMID);
    }
    
    return 0;
}
```

### 共享内存 + 信号同步
```c
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>

struct shared_data {
    int data;
    int ready;
};

void handler(int signum) {
    printf("收到信号: %d\n", signum);
}

int main() {
    key_t key;
    int shmid;
    struct shared_data *shared;
    pid_t pid;
    
    key = ftok("/tmp", 'A');
    shmid = shmget(key, sizeof(struct shared_data), IPC_CREAT | 0666);
    shared = (struct shared_data *)shmat(shmid, NULL, 0);
    
    shared->ready = 0;
    
    signal(SIGUSR1, handler);
    
    pid = fork();
    
    if (pid == 0) {
        // 子进程
        printf("子进程等待数据...\n");
        while (!shared->ready) {
            sleep(1);
        }
        printf("子进程收到数据: %d\n", shared->data);
        kill(getppid(), SIGUSR1);
    } else {
        // 父进程
        sleep(2);
        shared->data = 42;
        shared->ready = 1;
        printf("父进程发送数据: %d\n", shared->data);
        pause();
        printf("父进程收到确认\n");
        wait(NULL);
        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);
    }
    
    return 0;
}
```

---

## 线程间信号同步

### 线程间信号发送
```c
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

sigset_t mask;

void handler(int signum) {
    printf("线程 %ld 收到信号: %d\n", pthread_self(), signum);
}

void *thread_func(void *arg) {
    int sig;
    
    // 等待信号
    sigwait(&mask, &sig);
    printf("线程 %ld 处理信号: %d\n", pthread_self(), sig);
    
    return NULL;
}

int main() {
    pthread_t tid;
    
    // 设置信号掩码
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    
    // 注册信号处理函数
    signal(SIGUSR1, handler);
    
    // 创建线程
    pthread_create(&tid, NULL, thread_func, NULL);
    
    sleep(1);
    
    // 发送信号给线程
    pthread_kill(tid, SIGUSR1);
    
    pthread_join(tid, NULL);
    
    return 0;
}
```

### 线程间互斥锁同步
```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    pthread_create(&tid1, NULL, increment, NULL);
    pthread_create(&tid2, NULL, increment, NULL);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    printf("计数器值: %d\n", counter);
    
    return 0;
}
```

### 线程间条件变量同步
```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ready = 0;

void *waiter(void *arg) {
    pthread_mutex_lock(&mutex);
    
    while (!ready) {
        printf("等待线程: 等待条件...\n");
        pthread_cond_wait(&cond, &mutex);
    }
    
    printf("等待线程: 条件满足\n");
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

void *signaler(void *arg) {
    sleep(2);
    
    pthread_mutex_lock(&mutex);
    ready = 1;
    printf("通知线程: 设置条件\n");
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

int main() {
    pthread_t waiter_thread, signaler_thread;
    
    pthread_create(&waiter_thread, NULL, waiter, NULL);
    pthread_create(&signaler_thread, NULL, signaler, NULL);
    
    pthread_join(waiter_thread, NULL);
    pthread_join(signaler_thread, NULL);
    
    return 0;
}
```

---

## 高级同步技术

### 读写锁
```c
#include <pthread.h>

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);  // 读锁
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock); // 写锁
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
```

**示例:**
```c
#include <stdio.h>
#include <pthread.h>

pthread_rwlock_t rwlock;
int shared_data = 0;

void *reader(void *arg) {
    pthread_rwlock_rdlock(&rwlock);
    printf("读者 %ld 读取数据: %d\n", (long)arg, shared_data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void *writer(void *arg) {
    pthread_rwlock_wrlock(&rwlock);
    shared_data++;
    printf("写者 %ld 写入数据: %d\n", (long)arg, shared_data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

int main() {
    pthread_t threads[5];
    
    pthread_rwlock_init(&rwlock, NULL);
    
    // 创建读者和写者
    for (int i = 0; i < 5; i++) {
        if (i % 2 == 0) {
            pthread_create(&threads[i], NULL, reader, (void *)(i + 1));
        } else {
            pthread_create(&threads[i], NULL, writer, (void *)(i + 1));
        }
    }
    
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_rwlock_destroy(&rwlock);
    
    return 0;
}
```

### 自旋锁
```c
#include <pthread.h>

int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_destroy(pthread_spinlock_t *lock);
int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);
```

**示例:**
```c
#include <stdio.h>
#include <pthread.h>

pthread_spinlock_t spinlock;
int counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_spin_lock(&spinlock);
        counter++;
        pthread_spin_unlock(&spinlock);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
    
    pthread_create(&tid1, NULL, increment, NULL);
    pthread_create(&tid2, NULL, increment, NULL);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    printf("计数器值: %d\n", counter);
    
    pthread_spin_destroy(&spinlock);
    
    return 0;
}
```

### 屏障
```c
#include <pthread.h>

int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
int pthread_barrier_wait(pthread_barrier_t *barrier);
```

**示例:**
```c
#include <stdio.h>
#include <pthread.h>

pthread_barrier_t barrier;

void *worker(void *arg) {
    int id = (int)(long)arg;
    
    printf("线程 %d 开始工作\n", id);
    sleep(id);
    printf("线程 %d 完成工作，等待其他线程\n", id);
    
    pthread_barrier_wait(&barrier);
    
    printf("线程 %d 继续执行\n", id);
    
    return NULL;
}

int main() {
    pthread_t threads[3];
    
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
```

---

## 实际应用示例

### 信号处理服务器
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

volatile sig_atomic_t running = 1;

void sigint_handler(int signum) {
    printf("\n收到 SIGINT，准备退出...\n");
    running = 0;
}

void sigterm_handler(int signum) {
    printf("\n收到 SIGTERM，准备退出...\n");
    running = 0;
}

void setup_signals() {
    struct sigaction sa;
    
    // 设置 SIGINT 处理函数
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    
    // 设置 SIGTERM 处理函数
    sa.sa_handler = sigterm_handler;
    sigaction(SIGTERM, &sa, NULL);
    
    // 忽略 SIGPIPE
    signal(SIGPIPE, SIG_IGN);
}

int main() {
    setup_signals();
    
    printf("服务器启动，PID: %d\n", getpid());
    printf("按 Ctrl+C 或发送 SIGTERM 信号停止服务器\n");
    
    while (running) {
        // 服务器主循环
        printf("服务器运行中...\n");
        sleep(1);
    }
    
    printf("服务器已停止\n");
    
    return 0;
}
```

### 线程池实现
```c
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
```

### 进程间通信示例
```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>

struct shared_data {
    int buffer[10];
    int count;
    int in;
    int out;
};

int semid;
int shmid;
struct shared_data *shared;

void P(int semid, int sem_num) {
    struct sembuf op = {sem_num, -1, 0};
    semop(semid, &op, 1);
}

void V(int semid, int sem_num) {
    struct sembuf op = {sem_num, 1, 0};
    semop(semid, &op, 1);
}

void producer() {
    for (int i = 0; i < 20; i++) {
        P(semid, 1);  // 等待空位
        
        P(semid, 0);  // 获取互斥锁
        shared->buffer[shared->in] = i;
        printf("生产者生产: %d\n", i);
        shared->in = (shared->in + 1) % 10;
        shared->count++;
        V(semid, 0);  // 释放互斥锁
        
        V(semid, 2);  // 通知有数据
        
        sleep(1);
    }
}

void consumer() {
    for (int i = 0; i < 20; i++) {
        P(semid, 2);  // 等待数据
        
        P(semid, 0);  // 获取互斥锁
        int item = shared->buffer[shared->out];
        printf("消费者消费: %d\n", item);
        shared->out = (shared->out + 1) % 10;
        shared->count--;
        V(semid, 0);  // 释放互斥锁
        
        V(semid, 1);  // 通知有空位
        
        sleep(1);
    }
}

int main() {
    key_t key;
    pid_t pid;
    unsigned short sem_values[3] = {1, 10, 0};  // 互斥锁, 空位, 数据
    
    key = ftok("/tmp", 'A');
    
    // 创建信号量
    semid = semget(key, 3, IPC_CREAT | 0666);
    semctl(semid, 0, SETALL, sem_values);
    
    // 创建共享内存
    shmid = shmget(key, sizeof(struct shared_data), IPC_CREAT | 0666);
    shared = (struct shared_data *)shmat(shmid, NULL, 0);
    shared->count = 0;
    shared->in = 0;
    shared->out = 0;
    
    pid = fork();
    
    if (pid == 0) {
        producer();
    } else {
        consumer();
        wait(NULL);
        
        // 清理资源
        shmdt(shared);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);
    }
    
    return 0;
}
```

---

## 最佳实践

### 信号处理最佳实践

1. **保持信号处理函数简单**
```c
// 好的做法
volatile sig_atomic_t flag = 0;

void handler(int signum) {
    flag = 1;
}

// 避免的做法
void handler(int signum) {
    printf("处理信号\n");  // 不安全
    sleep(1);              // 不安全
}
```

2. **使用 sigaction 而不是 signal**
```c
// 好的做法
struct sigaction sa;
sa.sa_handler = handler;
sigemptyset(&sa.sa_mask);
sa.sa_flags = SA_RESTART;
sigaction(SIGINT, &sa, NULL);

// 避免的做法
signal(SIGINT, handler);
```

3. **在信号处理函数中使用异步安全函数**
```c
// 异步安全函数
write()
_exit()
kill()
sigprocmask()

// 非异步安全函数
printf()
malloc()
free()
```

4. **正确处理信号掩码**
```c
sigset_t mask, oldmask;

sigemptyset(&mask);
sigaddset(&mask, SIGINT);
sigprocmask(SIG_BLOCK, &mask, &oldmask);

// 临界区代码

sigprocmask(SIG_SETMASK, &oldmask, NULL);
```

### 互斥锁最佳实践

1. **总是检查返回值**
```c
if (pthread_mutex_lock(&mutex) != 0) {
    perror("pthread_mutex_lock");
    return -1;
}
```

2. **避免死锁**
```c
// 好的做法：按固定顺序获取锁
pthread_mutex_lock(&mutex1);
pthread_mutex_lock(&mutex2);
// 临界区
pthread_mutex_unlock(&mutex2);
pthread_mutex_unlock(&mutex1);

// 避免的做法：可能导致死锁
pthread_mutex_lock(&mutex1);
pthread_mutex_lock(&mutex2);
// ...
```

3. **使用 RAII 模式**
```c
#include <pthread.h>

class MutexLock {
private:
    pthread_mutex_t *mutex_;
public:
    MutexLock(pthread_mutex_t *mutex) : mutex_(mutex) {
        pthread_mutex_lock(mutex_);
    }
    ~MutexLock() {
        pthread_mutex_unlock(mutex_);
    }
};

// 使用
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
{
    MutexLock lock(&mutex);
    // 临界区
} // 自动解锁
```

### 条件变量最佳实践

1. **总是使用 while 循环检查条件**
```c
// 好的做法
pthread_mutex_lock(&mutex);
while (!condition) {
    pthread_cond_wait(&cond, &mutex);
}
// 处理条件
pthread_mutex_unlock(&mutex);

// 避免的做法
pthread_mutex_lock(&mutex);
if (!condition) {
    pthread_cond_wait(&cond, &mutex);
}
// 处理条件
pthread_mutex_unlock(&mutex);
```

2. **避免虚假唤醒**
```c
pthread_mutex_lock(&mutex);
while (!ready) {
    pthread_cond_wait(&cond, &mutex);
}
pthread_mutex_unlock(&mutex);
```

3. **正确使用超时**
```c
struct timespec ts;
clock_gettime(CLOCK_REALTIME, &ts);
ts.tv_sec += 5;

pthread_mutex_lock(&mutex);
while (!condition) {
    if (pthread_cond_timedwait(&cond, &mutex, &ts) == ETIMEDOUT) {
        // 处理超时
        break;
    }
}
pthread_mutex_unlock(&mutex);
```

### 性能优化建议

1. **减少锁的粒度**
```c
// 好的做法：细粒度锁
pthread_mutex_lock(&mutex1);
// 操作共享数据1
pthread_mutex_unlock(&mutex1);

pthread_mutex_lock(&mutex2);
// 操作共享数据2
pthread_mutex_unlock(&mutex2);

// 避免的做法：粗粒度锁
pthread_mutex_lock(&mutex);
// 操作共享数据1
// 操作共享数据2
pthread_mutex_unlock(&mutex);
```

2. **使用读写锁提高并发性**
```c
// 多读者场景
pthread_rwlock_rdlock(&rwlock);
// 读取数据
pthread_rwlock_unlock(&rwlock);

// 写者场景
pthread_rwlock_wrlock(&rwlock);
// 写入数据
pthread_rwlock_unlock(&rwlock);
```

3. **避免在临界区执行耗时操作**
```c
// 好的做法
pthread_mutex_lock(&mutex);
data = prepare_data();
pthread_mutex_unlock(&mutex);
process_data(data);

// 避免的做法
pthread_mutex_lock(&mutex);
data = prepare_data();
process_data(data);  // 耗时操作
pthread_mutex_unlock(&mutex);
```

---

## 故障排查

### 常见问题

#### 1. 信号丢失
```c
// 问题：信号在阻塞期间被发送
sigprocmask(SIG_BLOCK, &mask, NULL);
// 信号在此期间发送
sigprocmask(SIG_UNBLOCK, &mask, NULL);

// 解决：使用实时信号
sigqueue(pid, SIGRTMIN, value);
```

#### 2. 死锁
```c
// 问题：两个线程互相等待
// 线程1
pthread_mutex_lock(&mutex1);
pthread_mutex_lock(&mutex2);

// 线程2
pthread_mutex_lock(&mutex2);
pthread_mutex_lock(&mutex1);

// 解决：按固定顺序获取锁
pthread_mutex_lock(&mutex1);
pthread_mutex_lock(&mutex2);
```

#### 3. 竞态条件
```c
// 问题：检查和使用之间有时间窗口
if (condition) {
    // 条件可能在此改变
    use_resource();
}

// 解决：使用锁保护
pthread_mutex_lock(&mutex);
if (condition) {
    use_resource();
}
pthread_mutex_unlock(&mutex);
```

#### 4. 虚假唤醒
```c
// 问题：条件变量可能虚假唤醒
pthread_cond_wait(&cond, &mutex);

// 解决：使用 while 循环
while (!condition) {
    pthread_cond_wait(&cond, &mutex);
}
```

### 调试工具

#### 1. 使用 strace 跟踪系统调用
```bash
strace -e trace=signal ./program
```

#### 2. 使用 gdb 调试
```bash
gdb ./program
(gdb) handle SIGUSR1 print
(gdb) run
```

#### 3. 使用 pthread 调试
```bash
# 编译时添加调试选项
gcc -g -pthread program.c -o program

# 运行时使用调试器
gdb ./program
```

#### 4. 检查死锁
```bash
# 使用 pstack 查看线程堆栈
pstack <pid>

# 使用 gdb 查看锁状态
gdb -p <pid>
(gdb) info threads
(gdb) thread apply all bt
```

### 日志记录
```c
#include <stdio.h>
#include <time.h>

void log_message(const char *message) {
    time_t now;
    char timestamp[64];
    
    time(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    printf("[%s] %s\n", timestamp, message);
}

void log_lock(pthread_mutex_t *mutex, const char *operation) {
    char message[256];
    snprintf(message, sizeof(message), "线程 %ld %s 锁", pthread_self(), operation);
    log_message(message);
}
```

---

## 总结

Linux应用中的信号同步方法提供了多种机制来实现进程和线程间的协调：

1. **信号**: 异步通知机制，适合简单的事件通知
2. **信号量**: 计数同步机制，适合资源计数
3. **互斥锁**: 互斥访问机制，适合保护共享资源
4. **条件变量**: 条件等待机制，适合等待特定条件
5. **读写锁**: 读写分离机制，适合多读者场景
6. **自旋锁**: 忙等待机制，适合短时间锁定
7. **屏障**: 同步点机制，适合多线程同步

选择合适的同步方法需要考虑：
- 同步粒度
- 性能要求
- 复杂度
- 可维护性

通过合理使用这些同步机制，可以构建高效、可靠的多进程和多线程应用。