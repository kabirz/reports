# Linux pthread 线程同步方法完整指南

## 目录
1. [pthread 同步概述](#pthread-同步概述)
2. [互斥锁（Mutex）](#互斥锁mutex)
3. [条件变量（Condition Variable）](#条件变量condition-variable)
4. [读写锁（Read-Write Lock）](#读写锁read-write-lock)
5. [自旋锁（Spin Lock）](#自旋锁spin-lock)
6. [屏障（Barrier）](#屏障barrier)
7. [POSIX 信号量](#posix-信号量)
8. [线程特定信号处理](#线程特定信号处理)
9. [同步机制性能比较](#同步机制性能比较)
10. [实际应用场景](#实际应用场景)
11. [最佳实践](#最佳实践)
12. [常见问题和解决方案](#常见问题和解决方案)

---

## pthread 同步概述

### 为什么需要线程同步
在多线程程序中，多个线程可能同时访问共享资源，如果没有适当的同步机制，会导致：
- **竞态条件（Race Condition）**: 多个线程同时修改共享数据
- **数据不一致**: 读取到部分更新的数据
- **死锁（Deadlock）**: 线程互相等待对方释放资源
- **活锁（Livelock）**: 线程不断重试但无法进展
- **饥饿（Starvation）**: 某些线程长时间无法获得资源

### pthread 同步机制概览

| 同步机制 | 用途 | 特点 | 适用场景 |
|---------|------|------|---------|
| 互斥锁 | 保护临界区 | 互斥访问 | 保护共享数据 |
| 条件变量 | 等待条件成立 | 条件同步 | 生产者-消费者 |
| 读写锁 | 读写分离 | 多读者单写者 | 读多写少 |
| 自旋锁 | 忙等待 | 短时间锁定 | 短临界区 |
| 屏障 | 线程同步点 | 等待所有线程 | 并行计算 |
| 信号量 | 资源计数 | 计数同步 | 资源池管理 |

### 基本原则
1. **最小化临界区**: 只在必要时持有锁
2. **避免嵌套锁**: 减少死锁风险
3. **保持锁的顺序**: 按固定顺序获取多个锁
4. **及时释放锁**: 不要在临界区执行耗时操作
5. **使用适当的同步机制**: 根据场景选择最合适的机制

---

## 互斥锁（Mutex）

### 基本概念
互斥锁（Mutex，Mutual Exclusion）是最基本的同步机制，确保同一时间只有一个线程可以访问共享资源。

### 互斥锁类型

#### 1. 普通互斥锁（PTHREAD_MUTEX_NORMAL）
```c
#include <pthread.h>

pthread_mutex_t mutex;
pthread_mutexattr_t attr;

// 初始化属性
pthread_mutexattr_init(&attr);
pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);

// 初始化互斥锁
pthread_mutex_init(&mutex, &attr);

// 使用
pthread_mutex_lock(&mutex);
// 临界区
pthread_mutex_unlock(&mutex);

// 清理
pthread_mutex_destroy(&mutex);
pthread_mutexattr_destroy(&attr);
```

**特点:**
- 不提供错误检查
- 同一线程重复加锁会导致死锁
- 解锁未锁定的互斥锁会导致未定义行为

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
    pthread_mutexattr_t attr;
    
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&mutex, &attr);
    
    pthread_create(&tid1, NULL, increment, NULL);
    pthread_create(&tid2, NULL, increment, NULL);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    printf("计数器值: %d\n", counter);
    
    pthread_mutex_destroy(&mutex);
    pthread_mutexattr_destroy(&attr);
    
    return 0;
}
```

#### 2. 递归互斥锁（PTHREAD_MUTEX_RECURSIVE）
```c
#include <pthread.h>

pthread_mutex_t mutex;
pthread_mutexattr_t attr;

// 初始化递归互斥锁
pthread_mutexattr_init(&attr);
pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
pthread_mutex_init(&mutex, &attr);
```

**特点:**
- 同一线程可以多次加锁
- 必须调用相同次数的解锁
- 适合递归函数

**示例:**
```c
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
    
    recursive_function(5);
    
    pthread_mutex_destroy(&mutex);
    pthread_mutexattr_destroy(&attr);
    
    return 0;
}
```

#### 3. 错误检查互斥锁（PTHREAD_MUTEX_ERRORCHECK）
```c
#include <pthread.h>

pthread_mutex_t mutex;
pthread_mutexattr_t attr;

// 初始化错误检查互斥锁
pthread_mutexattr_init(&attr);
pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
pthread_mutex_init(&mutex, &attr);
```

**特点:**
- 检测错误并返回错误码
- 同一线程重复加锁返回 EDEADLK
- 解锁未锁定的互斥锁返回 EPERM

**示例:**
```c
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
```

#### 4. 默认互斥锁（PTHREAD_MUTEX_DEFAULT）
```c
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
```

**特点:**
- 行为取决于实现
- 通常等同于 PTHREAD_MUTEX_NORMAL
- 适合大多数场景

### 互斥锁操作

#### 基本操作
```c
#include <pthread.h>

// 初始化
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);

// 销毁
int pthread_mutex_destroy(pthread_mutex_t *mutex);

// 加锁
int pthread_mutex_lock(pthread_mutex_t *mutex);

// 尝试加锁
int pthread_mutex_trylock(pthread_mutex_t *mutex);

// 超时加锁
int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime);

// 解锁
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

#### 尝试加锁示例
```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex;

void *thread_func(void *arg) {
    int id = (int)(long)arg;
    
    if (pthread_mutex_trylock(&mutex) == 0) {
        printf("线程 %d 获得锁\n", id);
        sleep(2);
        printf("线程 %d 释放锁\n", id);
        pthread_mutex_unlock(&mutex);
    } else {
        printf("线程 %d 未能获得锁\n", id);
    }
    
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    pthread_mutex_init(&mutex, NULL);
    
    pthread_create(&tid1, NULL, thread_func, (void *)1);
    pthread_create(&tid2, NULL, thread_func, (void *)2);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
```

#### 超时加锁示例
```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

pthread_mutex_t mutex;

void *thread_func(void *arg) {
    int id = (int)(long)arg;
    struct timespec ts;
    
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 3;  // 3秒超时
    
    if (pthread_mutex_timedlock(&mutex, &ts) == 0) {
        printf("线程 %d 获得锁\n", id);
        sleep(2);
        pthread_mutex_unlock(&mutex);
    } else {
        printf("线程 %d 获取锁超时\n", id);
    }
    
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    pthread_mutex_init(&mutex, NULL);
    
    pthread_create(&tid1, NULL, thread_func, (void *)1);
    pthread_create(&tid2, NULL, thread_func, (void *)2);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
```

### 互斥锁属性

#### 进程共享属性
```c
#include <pthread.h>

pthread_mutexattr_t attr;

// 初始化属性
pthread_mutexattr_init(&attr);

// 设置进程共享
pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

// 初始化互斥锁
pthread_mutex_init(&mutex, &attr);
```

**示例:**
```c
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>

pthread_mutex_t *mutex;

int main() {
    pthread_mutexattr_t attr;
    
    // 创建共享内存
    mutex = mmap(NULL, sizeof(pthread_mutex_t), 
                 PROT_READ | PROT_WRITE, 
                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    // 初始化属性
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    
    // 初始化互斥锁
    pthread_mutex_init(mutex, &attr);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子进程
        pthread_mutex_lock(mutex);
        printf("子进程获得锁\n");
        sleep(2);
        printf("子进程释放锁\n");
        pthread_mutex_unlock(mutex);
    } else {
        // 父进程
        sleep(1);
        pthread_mutex_lock(mutex);
        printf("父进程获得锁\n");
        pthread_mutex_unlock(mutex);
        wait(NULL);
        
        // 清理
        pthread_mutex_destroy(mutex);
        pthread_mutexattr_destroy(&attr);
        munmap(mutex, sizeof(pthread_mutex_t));
    }
    
    return 0;
}
```

#### 优先级继承属性
```c
#include <pthread.h>

pthread_mutexattr_t attr;

// 初始化属性
pthread_mutexattr_init(&attr);

// 设置优先级继承
pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

// 初始化互斥锁
pthread_mutex_init(&mutex, &attr);
```

**协议类型:**
- `PTHREAD_PRIO_NONE`: 无优先级继承
- `PTHREAD_PRIO_INHERIT`: 优先级继承
- `PTHREAD_PRIO_PROTECT`: 优先级保护

### 静态初始化
```c
#include <pthread.h>

// 静态初始化
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// 使用
pthread_mutex_lock(&mutex);
// 临界区
pthread_mutex_unlock(&mutex);
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

## 条件变量（Condition Variable）

### 基本概念
条件变量用于线程间等待某个条件成立。它总是与互斥锁一起使用，以避免竞态条件。

### 基本操作

#### 初始化和销毁
```c
#include <pthread.h>

// 初始化
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);

// 销毁
int pthread_cond_destroy(pthread_cond_t *cond);

// 静态初始化
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
```

#### 等待和通知
```c
#include <pthread.h>

// 等待条件
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);

// 超时等待
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, 
                           const struct timespec *abstime);

// 唤醒一个等待线程
int pthread_cond_signal(pthread_cond_t *cond);

// 唤醒所有等待线程
int pthread_cond_broadcast(pthread_cond_t *cond);
```

### 生产者-消费者模型

#### 基本实现
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
        
        // 等待缓冲区不满
        while (count == BUFFER_SIZE) {
            printf("缓冲区满，生产者等待\n");
            pthread_cond_wait(&not_full, &mutex);
        }
        
        // 生产数据
        buffer[in] = i;
        printf("生产者生产: %d\n", i);
        in = (in + 1) % BUFFER_SIZE;
        count++;
        
        // 通知消费者
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);
        
        // 等待缓冲区不空
        while (count == 0) {
            printf("缓冲区空，消费者等待\n");
            pthread_cond_wait(&not_empty, &mutex);
        }
        
        // 消费数据
        int item = buffer[out];
        printf("消费者消费: %d\n", item);
        out = (out + 1) % BUFFER_SIZE;
        count--;
        
        // 通知生产者
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
#include <errno.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int condition = 0;

void *waiter(void *arg) {
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
```

### 多生产者多消费者
```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define PRODUCER_COUNT 3
#define CONSUMER_COUNT 3

int buffer[BUFFER_SIZE];
int count = 0;
int in = 0, out = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    int id = (int)(long)arg;
    int item = 0;
    
    while (1) {
        pthread_mutex_lock(&mutex);
        
        while (count == BUFFER_SIZE) {
            printf("生产者 %d: 缓冲区满，等待\n", id);
            pthread_cond_wait(&not_full, &mutex);
        }
        
        buffer[in] = item;
        printf("生产者 %d: 生产 %d\n", id, item);
        in = (in + 1) % BUFFER_SIZE;
        count++;
        item++;
        
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = (int)(long)arg;
    
    while (1) {
        pthread_mutex_lock(&mutex);
        
        while (count == 0) {
            printf("消费者 %d: 缓冲区空，等待\n", id);
            pthread_cond_wait(&not_empty, &mutex);
        }
        
        int item = buffer[out];
        printf("消费者 %d: 消费 %d\n", id, item);
        out = (out + 1) % BUFFER_SIZE;
        count--;
        
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t producers[PRODUCER_COUNT];
    pthread_t consumers[CONSUMER_COUNT];
    
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_create(&producers[i], NULL, producer, (void *)(long)i);
    }
    
    for (int i = 0; i < CONSUMER_COUNT; i++) {
        pthread_create(&consumers[i], NULL, consumer, (void *)(long)i);
    }
    
    // 让程序运行一段时间
    sleep(30);
    
    return 0;
}
```

### 条件变量属性

#### 时钟属性
```c
#include <pthread.h>

pthread_condattr_t attr;

// 初始化属性
pthread_condattr_init(&attr);

// 设置时钟
pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);

// 初始化条件变量
pthread_cond_init(&cond, &attr);
```

**示例:**
```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>

pthread_cond_t cond;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    pthread_condattr_t attr;
    struct timespec ts;
    
    // 初始化属性
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    
    // 初始化条件变量
    pthread_cond_init(&cond, &attr);
    
    // 使用单调时钟进行超时等待
    pthread_mutex_lock(&mutex);
    
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ts.tv_sec += 5;
    
    pthread_cond_timedwait(&cond, &mutex, &ts);
    
    pthread_mutex_unlock(&mutex);
    
    // 清理
    pthread_cond_destroy(&cond);
    pthread_condattr_destroy(&attr);
    
    return 0;
}
```

---

## 读写锁（Read-Write Lock）

### 基本概念
读写锁允许多个读者同时访问共享资源，但写者独占访问。适合读多写少的场景。

### 基本操作

#### 初始化和销毁
```c
#include <pthread.h>

// 初始化
int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);

// 销毁
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);

// 静态初始化
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
```

#### 读锁操作
```c
#include <pthread.h>

// 获取读锁
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);

// 尝试获取读锁
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);

// 超时获取读锁
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);
```

#### 写锁操作
```c
#include <pthread.h>

// 获取写锁
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);

// 尝试获取写锁
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);

// 超时获取写锁
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);

// 解锁
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
```

### 基本示例
```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_rwlock_t rwlock;
int shared_data = 0;

void *reader(void *arg) {
    int id = (int)(long)arg;
    
    pthread_rwlock_rdlock(&rwlock);
    printf("读者 %d: 读取数据 %d\n", id, shared_data);
    sleep(1);
    printf("读者 %d: 完成读取\n", id);
    pthread_rwlock_unlock(&rwlock);
    
    return NULL;
}

void *writer(void *arg) {
    int id = (int)(long)arg;
    
    pthread_rwlock_wrlock(&rwlock);
    printf("写者 %d: 开始写入\n", id);
    shared_data++;
    sleep(2);
    printf("写者 %d: 完成写入，数据 = %d\n", id, shared_data);
    pthread_rwlock_unlock(&rwlock);
    
    return NULL;
}

int main() {
    pthread_t threads[5];
    
    pthread_rwlock_init(&rwlock, NULL);
    
    // 创建读者和写者
    pthread_create(&threads[0], NULL, reader, (void *)1);
    pthread_create(&threads[1], NULL, reader, (void *)2);
    pthread_create(&threads[2], NULL, writer, (void *)1);
    pthread_create(&threads[3], NULL, reader, (void *)3);
    pthread_create(&threads[4], NULL, writer, (void *)2);
    
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_rwlock_destroy(&rwlock);
    
    return 0;
}
```

### 读写锁属性

#### 优先级属性
```c
#include <pthread.h>

pthread_rwlockattr_t attr;

// 初始化属性
pthread_rwlockattr_init(&attr);

// 设置优先级
pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_READER_NP);

// 初始化读写锁
pthread_rwlock_init(&rwlock, &attr);
```

**优先级类型:**
- `PTHREAD_RWLOCK_PREFER_READER_NP`: 读者优先（可能导致写者饥饿）
- `PTHREAD_RWLOCK_PREFER_WRITER_NP`: 写者优先
- `PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP`: 写者优先，非递归

**示例:**
```c
#include <stdio.h>
#include <pthread.h>

pthread_rwlock_t rwlock;
pthread_rwlockattr_t attr;

int main() {
    // 初始化属性
    pthread_rwlockattr_init(&attr);
    pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NP);
    
    // 初始化读写锁
    pthread_rwlock_init(&rwlock, &attr);
    
    // 使用读写锁
    pthread_rwlock_rdlock(&rwlock);
    // 读取操作
    pthread_rwlock_unlock(&rwlock);
    
    // 清理
    pthread_rwlock_destroy(&rwlock);
    pthread_rwlockattr_destroy(&attr);
    
    return 0;
}
```

### 缓存一致性示例
```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define DATA_SIZE 1000

int data[DATA_SIZE];
pthread_rwlock_t rwlock;

void *reader(void *arg) {
    int id = (int)(long)arg;
    int sum = 0;
    
    pthread_rwlock_rdlock(&rwlock);
    
    for (int i = 0; i < DATA_SIZE; i++) {
        sum += data[i];
    }
    
    printf("读者 %d: 总和 = %d\n", id, sum);
    
    pthread_rwlock_unlock(&rwlock);
    
    return NULL;
}

void *writer(void *arg) {
    int id = (int)(long)arg;
    
    pthread_rwlock_wrlock(&rwlock);
    
    for (int i = 0; i < DATA_SIZE; i++) {
        data[i] = i * id;
    }
    
    printf("写者 %d: 更新数据完成\n", id);
    
    pthread_rwlock_unlock(&rwlock);
    
    return NULL;
}

int main() {
    pthread_t threads[4];
    
    pthread_rwlock_init(&rwlock, NULL);
    
    // 初始化数据
    for (int i = 0; i < DATA_SIZE; i++) {
        data[i] = i;
    }
    
    // 创建读者和写者
    pthread_create(&threads[0], NULL, reader, (void *)1);
    pthread_create(&threads[1], NULL, reader, (void *)2);
    pthread_create(&threads[2], NULL, writer, (void *)1);
    pthread_create(&threads[3], NULL, reader, (void *)3);
    
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_rwlock_destroy(&rwlock);
    
    return 0;
}
```

---

## 自旋锁（Spin Lock）

### 基本概念
自旋锁是一种忙等待的锁机制，线程在获取锁时会循环检查锁是否可用。适合临界区非常短的场景。

### 基本操作

#### 初始化和销毁
```c
#include <pthread.h>

// 初始化
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);

// 销毁
int pthread_spin_destroy(pthread_spinlock_t *lock);
```

#### 加锁和解锁
```c
#include <pthread.h>

// 加锁
int pthread_spin_lock(pthread_spinlock_t *lock);

// 尝试加锁
int pthread_spin_trylock(pthread_spinlock_t *lock);

// 解锁
int pthread_spin_unlock(pthread_spinlock_t *lock);
```

### 基本示例
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

### 自旋锁 vs 互斥锁
```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define ITERATIONS 10000000

pthread_mutex_t mutex;
pthread_spinlock_t spinlock;
int counter_mutex = 0;
int counter_spin = 0;

void *increment_mutex(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        counter_mutex++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *increment_spin(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_spin_lock(&spinlock);
        counter_spin++;
        pthread_spin_unlock(&spinlock);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    struct timespec start, end;
    double time_mutex, time_spin;
    
    // 测试互斥锁
    pthread_mutex_init(&mutex, NULL);
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_create(&tid1, NULL, increment_mutex, NULL);
    pthread_create(&tid2, NULL, increment_mutex, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    time_mutex = (end.tv_sec - start.tv_sec) + 
                 (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("互斥锁: 计数器 = %d, 时间 = %.3f 秒\n", 
           counter_mutex, time_mutex);
    
    pthread_mutex_destroy(&mutex);
    
    // 测试自旋锁
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_create(&tid1, NULL, increment_spin, NULL);
    pthread_create(&tid2, NULL, increment_spin, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    time_spin = (end.tv_sec - start.tv_sec) + 
                (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("自旋锁: 计数器 = %d, 时间 = %.3f 秒\n", 
           counter_spin, time_spin);
    
    pthread_spin_destroy(&spinlock);
    
    return 0;
}
```

### 使用场景
```c
#include <stdio.h>
#include <pthread.h>

// 适合自旋锁的场景：非常短的临界区
pthread_spinlock_t spinlock;
int shared_counter = 0;

void *fast_increment(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        // 非常短的临界区
        pthread_spin_lock(&spinlock);
        shared_counter++;
        pthread_spin_unlock(&spinlock);
    }
    return NULL;
}

// 不适合自旋锁的场景：较长的临界区
pthread_mutex_t mutex;
int shared_data[1000];

void *slow_operation(void *arg) {
    pthread_mutex_lock(&mutex);
    
    // 较长的临界区
    for (int i = 0; i < 1000; i++) {
        shared_data[i] = i * 2;
    }
    
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    // 使用自旋锁
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
    pthread_create(&tid1, NULL, fast_increment, NULL);
    pthread_create(&tid2, NULL, fast_increment, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_spin_destroy(&spinlock);
    
    // 使用互斥锁
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&tid1, NULL, slow_operation, NULL);
    pthread_create(&tid2, NULL, slow_operation, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
```

---

## 屏障（Barrier）

### 基本概念
屏障是一种同步机制，用于等待一组线程到达某个同步点。所有线程到达屏障后，才能继续执行。

### 基本操作

#### 初始化和销毁
```c
#include <pthread.h>

// 初始化
int pthread_barrier_init(pthread_barrier_t *barrier, 
                        const pthread_barrierattr_t *attr, 
                        unsigned count);

// 销毁
int pthread_barrier_destroy(pthread_barrier_t *barrier);
```

#### 等待屏障
```c
#include <pthread.h>

// 等待屏障
int pthread_barrier_wait(pthread_barrier_t *barrier);
```

### 基本示例
```c
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
```

### 多阶段并行计算
```c
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000
#define THREADS 4

int data[SIZE];
int result[SIZE];
pthread_barrier_t barrier;

void *parallel_compute(void *arg) {
    int id = (int)(long)arg;
    int start = id * (SIZE / THREADS);
    int end = (id + 1) * (SIZE / THREADS);
    
    // 阶段1: 初始化数据
    for (int i = start; i < end; i++) {
        data[i] = i;
    }
    
    printf("线程 %d: 阶段1完成\n", id);
    pthread_barrier_wait(&barrier);
    
    // 阶段2: 计算平方
    for (int i = start; i < end; i++) {
        result[i] = data[i] * data[i];
    }
    
    printf("线程 %d: 阶段2完成\n", id);
    pthread_barrier_wait(&barrier);
    
    // 阶段3: 计算总和
    int sum = 0;
    for (int i = start; i < end; i++) {
        sum += result[i];
    }
    
    printf("线程 %d: 阶段3完成，局部和 = %d\n", id, sum);
    
    return NULL;
}

int main() {
    pthread_t threads[THREADS];
    
    pthread_barrier_init(&barrier, NULL, THREADS);
    
    for (int i = 0; i < THREADS; i++) {
        pthread_create(&threads[i], NULL, parallel_compute, (void *)(long)i);
    }
    
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_barrier_destroy(&barrier);
    
    return 0;
}
```

### 屏障属性

#### 进程共享属性
```c
#include <pthread.h>

pthread_barrierattr_t attr;

// 初始化属性
pthread_barrierattr_init(&attr);

// 设置进程共享
pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

// 初始化屏障
pthread_barrier_init(&barrier, &attr, count);
```

---

## POSIX 信号量

### 基本概念
信号量是一种计数同步机制，用于控制对共享资源的访问。POSIX信号量分为命名信号量和未命名信号量。

### 未命名信号量

#### 初始化和销毁
```c
#include <semaphore.h>

// 初始化
int sem_init(sem_t *sem, int pshared, unsigned int value);

// 销毁
int sem_destroy(sem_t *sem);
```

#### 操作
```c
#include <semaphore.h>

// 等待（P操作）
int sem_wait(sem_t *sem);

// 尝试等待
int sem_trywait(sem_t *sem);

// 超时等待
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);

// 释放（V操作）
int sem_post(sem_t *sem);

// 获取值
int sem_getvalue(sem_t *sem, int *sval);
```

### 基本示例
```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t empty_slots;
sem_t full_slots;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer(void *arg) {
    int id = (int)(long)arg;
    int item = 0;
    
    while (1) {
        sem_wait(&empty_slots);
        
        pthread_mutex_lock(&mutex);
        buffer[in] = item;
        printf("生产者 %d: 生产 %d\n", id, item);
        in = (in + 1) % BUFFER_SIZE;
        item++;
        pthread_mutex_unlock(&mutex);
        
        sem_post(&full_slots);
        
        sleep(1);
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = (int)(long)arg;
    
    while (1) {
        sem_wait(&full_slots);
        
        pthread_mutex_lock(&mutex);
        int item = buffer[out];
        printf("消费者 %d: 消费 %d\n", id, item);
        out = (out + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);
        
        sem_post(&empty_slots);
        
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;
    
    sem_init(&empty_slots, 0, BUFFER_SIZE);
    sem_init(&full_slots, 0, 0);
    
    pthread_create(&producer_thread, NULL, producer, (void *)1);
    pthread_create(&consumer_thread, NULL, consumer, (void *)1);
    
    sleep(30);
    
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    
    return 0;
}
```

### 命名信号量

#### 创建和打开
```c
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

// 创建或打开
sem_t *sem_open(const char *name, int oflag, ...);

// 关闭
int sem_close(sem_t *sem);

// 删除
int sem_unlink(const char *name);
```

### 命名信号量示例
```c
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
```

### 资源池管理
```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define RESOURCE_COUNT 3
#define THREAD_COUNT 5

sem_t resource_sem;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int resources[RESOURCE_COUNT];

void *worker(void *arg) {
    int id = (int)(long)arg;
    int resource_id;
    
    // 等待可用资源
    sem_wait(&resource_sem);
    
    // 分配资源
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        if (resources[i] == 0) {
            resources[i] = id;
            resource_id = i;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    
    printf("线程 %d: 使用资源 %d\n", id, resource_id);
    sleep(2);
    
    // 释放资源
    pthread_mutex_lock(&mutex);
    resources[resource_id] = 0;
    pthread_mutex_unlock(&mutex);
    
    printf("线程 %d: 释放资源 %d\n", id, resource_id);
    sem_post(&resource_sem);
    
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    
    sem_init(&resource_sem, 0, RESOURCE_COUNT);
    
    for (int i = 0; i < RESOURCE_COUNT; i++) {
        resources[i] = 0;
    }
    
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, worker, (void *)(long)i);
    }
    
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
    
    sem_destroy(&resource_sem);
    
    return 0;
}
```

---

## 线程特定信号处理

### 基本概念
线程可以有自己的信号掩码，可以独立处理信号。使用 `pthread_sigmask()` 和 `sigwait()` 可以实现线程特定的信号处理。

### 信号掩码操作

#### 设置线程信号掩码
```c
#include <signal.h>
#include <pthread.h>

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset);
```

**参数说明:**
- `how`: 操作方式
  - `SIG_BLOCK`: 添加信号到掩码
  - `SIG_UNBLOCK`: 从掩码中移除信号
  - `SIG_SETMASK`: 设置新的掩码

### 基本示例
```c
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

sigset_t mask;

void *signal_thread(void *arg) {
    int sig;
    
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
```

### 线程间信号通信
```c
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

sigset_t mask;
volatile sig_atomic_t flag = 0;

void *receiver_thread(void *arg) {
    int sig;
    
    printf("接收线程: 等待信号...\n");
    
    while (1) {
        if (sigwait(&mask, &sig) == 0) {
            printf("接收线程: 收到信号 %d\n", sig);
            flag = 1;
            break;
        }
    }
    
    return NULL;
}

void *sender_thread(void *arg) {
    int id = (int)(long)arg;
    
    sleep(2);
    
    printf("发送线程 %d: 发送信号\n", id);
    pthread_kill(pthread_self(), SIGUSR1);
    
    return NULL;
}

int main() {
    pthread_t receiver_tid, sender_tid;
    
    // 设置信号掩码
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    
    // 创建接收线程
    pthread_create(&receiver_tid, NULL, receiver_thread, NULL);
    
    // 创建发送线程
    pthread_create(&sender_tid, NULL, sender_thread, (void *)1);
    
    pthread_join(sender_tid, NULL);
    pthread_join(receiver_tid, NULL);
    
    if (flag) {
        printf("主线程: 信号已处理\n");
    }
    
    return 0;
}
```

### 定时器信号
```c
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

timer_t timerid;
volatile sig_atomic_t timer_flag = 0;

void timer_handler(int sig) {
    printf("定时器信号: 收到 SIGRTMIN\n");
    timer_flag = 1;
}

void *timer_thread(void *arg) {
    struct sigevent sev;
    struct itimerspec its;
    sigset_t mask;
    int sig;
    
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
```

---

## 同步机制性能比较

### 性能测试框架
```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>

#define ITERATIONS 1000000
#define THREADS 4

// 测试数据结构
typedef struct {
    pthread_mutex_t mutex;
    pthread_spinlock_t spinlock;
    pthread_rwlock_t rwlock;
    sem_t semaphore;
    int counter;
} test_data_t;

// 互斥锁测试
void *test_mutex(void *arg) {
    test_data_t *data = (test_data_t *)arg;
    
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&data->mutex);
        data->counter++;
        pthread_mutex_unlock(&data->mutex);
    }
    
    return NULL;
}

// 自旋锁测试
void *test_spinlock(void *arg) {
    test_data_t *data = (test_data_t *)arg;
    
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_spin_lock(&data->spinlock);
        data->counter++;
        pthread_spin_unlock(&data->spinlock);
    }
    
    return NULL;
}

// 读写锁测试（写锁）
void *test_rwlock(void *arg) {
    test_data_t *data = (test_data_t *)arg;
    
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_rwlock_wrlock(&data->rwlock);
        data->counter++;
        pthread_rwlock_unlock(&data->rwlock);
    }
    
    return NULL;
}

// 信号量测试
void *test_semaphore(void *arg) {
    test_data_t *data = (test_data_t *)arg;
    
    for (int i = 0; i < ITERATIONS; i++) {
        sem_wait(&data->semaphore);
        data->counter++;
        sem_post(&data->semaphore);
    }
    
    return NULL;
}

// 性能测试函数
double run_test(void *(*test_func)(void *), test_data_t *data) {
    pthread_t threads[THREADS];
    struct timespec start, end;
    
    data->counter = 0;
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < THREADS; i++) {
        pthread_create(&threads[i], NULL, test_func, data);
    }
    
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    test_data_t data;
    double time_mutex, time_spinlock, time_rwlock, time_semaphore;
    
    // 初始化同步机制
    pthread_mutex_init(&data.mutex, NULL);
    pthread_spin_init(&data.spinlock, PTHREAD_PROCESS_PRIVATE);
    pthread_rwlock_init(&data.rwlock, NULL);
    sem_init(&data.semaphore, 0, 1);
    
    printf("性能测试: %d 线程，每线程 %d 次迭代\n", THREADS, ITERATIONS);
    printf("------------------------------------------------\n");
    
    // 测试互斥锁
    time_mutex = run_test(test_mutex, &data);
    printf("互斥锁:   计数器 = %d, 时间 = %.3f 秒\n", 
           data.counter, time_mutex);
    
    // 测试自旋锁
    time_spinlock = run_test(test_spinlock, &data);
    printf("自旋锁:   计数器 = %d, 时间 = %.3f 秒\n", 
           data.counter, time_spinlock);
    
    // 测试读写锁
    time_rwlock = run_test(test_rwlock, &data);
    printf("读写锁:   计数器 = %d, 时间 = %.3f 秒\n", 
           data.counter, time_rwlock);
    
    // 测试信号量
    time_semaphore = run_test(test_semaphore, &data);
    printf("信号量:   计数器 = %d, 时间 = %.3f 秒\n", 
           data.counter, time_semaphore);
    
    printf("------------------------------------------------\n");
    printf("相对性能:\n");
    printf("互斥锁:   1.00x (基准)\n");
    printf("自旋锁:   %.2fx\n", time_mutex / time_spinlock);
    printf("读写锁:   %.2fx\n", time_mutex / time_rwlock);
    printf("信号量:   %.2fx\n", time_mutex / time_semaphore);
    
    // 清理
    pthread_mutex_destroy(&data.mutex);
    pthread_spin_destroy(&data.spinlock);
    pthread_rwlock_destroy(&data.rwlock);
    sem_destroy(&data.semaphore);
    
    return 0;
}
```

### 性能比较总结

| 同步机制 | 适用场景 | 性能 | 开销 |
|---------|---------|------|------|
| 互斥锁 | 通用场景 | 中等 | 中等 |
| 自旋锁 | 短临界区 | 高（短临界区） | 低（短临界区） |
| 读写锁 | 读多写少 | 高（多读者） | 中等 |
| 信号量 | 资源计数 | 中等 | 中等 |
| 条件变量 | 条件同步 | 中等 | 中等 |

---

## 实际应用场景

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

### 并行排序
```c
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10000
#define THREADS 4

int array[SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void merge(int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    int L[n1], R[n2];
    
    for (int i = 0; i < n1; i++)
        L[i] = array[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = array[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            array[k] = L[i];
            i++;
        } else {
            array[k] = R[j];
            j++;
        }
        k++;
    }
    
    while (i < n1) {
        array[k] = L[i];
        i++;
        k++;
    }
    
    while (j < n2) {
        array[k] = R[j];
        j++;
        k++;
    }
}

void merge_sort(int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(left, mid);
        merge_sort(mid + 1, right);
        merge(left, mid, right);
    }
}

void *parallel_merge_sort(void *arg) {
    int id = (int)(long)arg;
    int start = id * (SIZE / THREADS);
    int end = (id + 1) * (SIZE / THREADS) - 1;
    
    merge_sort(start, end);
    
    return NULL;
}

int main() {
    pthread_t threads[THREADS];
    struct timespec start, end;
    double time_parallel, time_sequential;
    
    // 初始化数组
    srand(time(NULL));
    for (int i = 0; i < SIZE; i++) {
        array[i] = rand() % 10000;
    }
    
    // 并行排序
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < THREADS; i++) {
        pthread_create(&threads[i], NULL, parallel_merge_sort, (void *)(long)i);
    }
    
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // 合并结果
    for (int i = 1; i < THREADS; i++) {
        merge(0, i * (SIZE / THREADS) - 1, (i + 1) * (SIZE / THREADS) - 1);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_parallel = (end.tv_sec - start.tv_sec) + 
                    (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("并行排序: %.3f 秒\n", time_parallel);
    
    // 重新初始化数组
    for (int i = 0; i < SIZE; i++) {
        array[i] = rand() % 10000;
    }
    
    // 顺序排序
    clock_gettime(CLOCK_MONOTONIC, &start);
    merge_sort(0, SIZE - 1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_sequential = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("顺序排序: %.3f 秒\n", time_sequential);
    printf("加速比: %.2fx\n", time_sequential / time_parallel);
    
    return 0;
}
```

### 并行矩阵乘法
```c
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000
#define THREADS 4

int matrix_a[SIZE][SIZE];
int matrix_b[SIZE][SIZE];
int matrix_c[SIZE][SIZE];

typedef struct {
    int start_row;
    int end_row;
} thread_data_t;

void *multiply(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < SIZE; j++) {
            matrix_c[i][j] = 0;
            for (int k = 0; k < SIZE; k++) {
                matrix_c[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }
    
    return NULL;
}

int main() {
    pthread_t threads[THREADS];
    thread_data_t data[THREADS];
    struct timespec start, end;
    double time_parallel, time_sequential;
    
    // 初始化矩阵
    srand(time(NULL));
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            matrix_a[i][j] = rand() % 100;
            matrix_b[i][j] = rand() % 100;
        }
    }
    
    // 并行乘法
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < THREADS; i++) {
        data[i].start_row = i * (SIZE / THREADS);
        data[i].end_row = (i + 1) * (SIZE / THREADS);
        pthread_create(&threads[i], NULL, multiply, &data[i]);
    }
    
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_parallel = (end.tv_sec - start.tv_sec) + 
                    (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("并行乘法: %.3f 秒\n", time_parallel);
    
    // 顺序乘法
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            matrix_c[i][j] = 0;
            for (int k = 0; k < SIZE; k++) {
                matrix_c[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_sequential = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("顺序乘法: %.3f 秒\n", time_sequential);
    printf("加速比: %.2fx\n", time_sequential / time_parallel);
    
    return 0;
}
```

---

## 最佳实践

### 1. 选择合适的同步机制

```c
// 场景1: 简单的互斥访问
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_lock(&mutex);
// 临界区
pthread_mutex_unlock(&mutex);

// 场景2: 读多写少
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

// 读者
pthread_rwlock_rdlock(&rwlock);
// 读取操作
pthread_rwlock_unlock(&rwlock);

// 写者
pthread_rwlock_wrlock(&rwlock);
// 写入操作
pthread_rwlock_unlock(&rwlock);

// 场景3: 等待条件
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_lock(&mutex);
while (!condition) {
    pthread_cond_wait(&cond, &mutex);
}
// 条件满足
pthread_mutex_unlock(&mutex);

// 场景4: 资源计数
sem_t semaphore;
sem_init(&semaphore, 0, RESOURCE_COUNT);

sem_wait(&semaphore);
// 使用资源
sem_post(&semaphore);
```

### 2. 避免死锁

```c
// 好的做法：按固定顺序获取锁
pthread_mutex_lock(&mutex1);
pthread_mutex_lock(&mutex2);
// 临界区
pthread_mutex_unlock(&mutex2);
pthread_mutex_unlock(&mutex1);

// 避免的做法：可能导致死锁
// 线程1
pthread_mutex_lock(&mutex1);
pthread_mutex_lock(&mutex2);

// 线程2
pthread_mutex_lock(&mutex2);
pthread_mutex_lock(&mutex1);
```

### 3. 最小化临界区

```c
// 好的做法：最小化临界区
pthread_mutex_lock(&mutex);
data = prepare_data();
pthread_mutex_unlock(&mutex);
process_data(data);

// 避免的做法：临界区过大
pthread_mutex_lock(&mutex);
data = prepare_data();
process_data(data);  // 耗时操作
pthread_mutex_unlock(&mutex);
```

### 4. 使用 RAII 模式

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

### 5. 错误处理

```c
#include <stdio.h>
#include <pthread.h>

int main() {
    pthread_mutex_t mutex;
    int ret;
    
    ret = pthread_mutex_init(&mutex, NULL);
    if (ret != 0) {
        fprintf(stderr, "互斥锁初始化失败: %d\n", ret);
        return 1;
    }
    
    ret = pthread_mutex_lock(&mutex);
    if (ret != 0) {
        fprintf(stderr, "加锁失败: %d\n", ret);
        pthread_mutex_destroy(&mutex);
        return 1;
    }
    
    // 临界区
    
    ret = pthread_mutex_unlock(&mutex);
    if (ret != 0) {
        fprintf(stderr, "解锁失败: %d\n", ret);
    }
    
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
```

### 6. 条件变量最佳实践

```c
// 好的做法：使用 while 循环
pthread_mutex_lock(&mutex);
while (!condition) {
    pthread_cond_wait(&cond, &mutex);
}
// 处理条件
pthread_mutex_unlock(&mutex);

// 避免的做法：使用 if 语句
pthread_mutex_lock(&mutex);
if (!condition) {
    pthread_cond_wait(&cond, &mutex);
}
// 处理条件
pthread_mutex_unlock(&mutex);
```

---

## 常见问题和解决方案

### 1. 死锁

**问题:**
```c
// 线程1
pthread_mutex_lock(&mutex1);
pthread_mutex_lock(&mutex2);

// 线程2
pthread_mutex_lock(&mutex2);
pthread_mutex_lock(&mutex1);
```

**解决方案:**
```c
// 按固定顺序获取锁
pthread_mutex_lock(&mutex1);
pthread_mutex_lock(&mutex2);
// 临界区
pthread_mutex_unlock(&mutex2);
pthread_mutex_unlock(&mutex1);
```

### 2. 竞态条件

**问题:**
```c
if (condition) {
    // 条件可能在此改变
    use_resource();
}
```

**解决方案:**
```c
pthread_mutex_lock(&mutex);
if (condition) {
    use_resource();
}
pthread_mutex_unlock(&mutex);
```

### 3. 虚假唤醒

**问题:**
```c
pthread_cond_wait(&cond, &mutex);
// 可能虚假唤醒
```

**解决方案:**
```c
while (!condition) {
    pthread_cond_wait(&cond, &mutex);
}
```

### 4. 优先级反转

**问题:**
高优先级线程等待低优先级线程持有的锁，而低优先级线程被中优先级线程抢占。

**解决方案:**
```c
// 使用优先级继承
pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);
pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
pthread_mutex_init(&mutex, &attr);
```

### 5. 锁的粒度

**问题:**
锁的粒度过大或过小都会影响性能。

**解决方案:**
```c
// 适当的锁粒度
pthread_mutex_lock(&mutex1);
// 操作共享数据1
pthread_mutex_unlock(&mutex1);

pthread_mutex_lock(&mutex2);
// 操作共享数据2
pthread_mutex_unlock(&mutex2);
```

---

## 总结

Linux pthread 提供了丰富的同步机制，每种机制都有其特定的适用场景：

1. **互斥锁**: 最基本的同步机制，适合大多数场景
2. **条件变量**: 用于等待条件成立，适合生产者-消费者模型
3. **读写锁**: 适合读多写少的场景
4. **自旋锁**: 适合短临界区，避免上下文切换
5. **屏障**: 用于线程同步点，适合并行计算
6. **信号量**: 用于资源计数，适合资源池管理

选择合适的同步机制需要考虑：
- 访问模式（读多写少、读写均衡）
- 临界区长度
- 性能要求
- 复杂度
- 可维护性

通过合理使用这些同步机制，可以构建高效、可靠的多线程应用。