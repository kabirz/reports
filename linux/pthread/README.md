# Linux pthread 线程同步示例代码

本目录包含了 Linux pthread 线程同步方法的完整示例代码，每个子目录对应一种同步机制或应用场景。

## 目录结构

```
./
├── mutex/              # 互斥锁（Mutex）示例
├── condition/           # 条件变量（Condition Variable）示例
├── rwlock/             # 读写锁（Read-Write Lock）示例
├── spinlock/           # 自旋锁（Spin Lock）示例
├── barrier/            # 屏障（Barrier）示例
├── semaphore/          # POSIX 信号量示例
├── signal/             # 线程特定信号处理示例
├── performance/         # 性能比较示例
├── application/        # 实际应用场景示例
└── best_practices/     # 最佳实践示例
```

## 编译和运行

每个目录都包含一个 Makefile，可以使用以下命令编译和运行：

```bash
# 进入特定目录
cd ./mutex

# 编译所有示例
make

# 运行特定示例
./01_normal_mutex

# 清理编译文件
make clean
```

## 示例说明

### 1. 互斥锁（Mutex）
- `01_normal_mutex.c` - 普通互斥锁示例
- `02_recursive_mutex.c` - 递归互斥锁示例
- `03_errorcheck_mutex.c` - 错误检查互斥锁示例
- `04_trylock_mutex.c` - 尝试加锁示例
- `05_timedlock_mutex.c` - 超时加锁示例
- `06_static_init_mutex.c` - 静态初始化示例

### 2. 条件变量（Condition Variable）
- `01_producer_consumer.c` - 生产者-消费者模型
- `02_timedwait.c` - 超时等待示例
- `03_multi_producer_consumer.c` - 多生产者多消费者模型

### 3. 读写锁（Read-Write Lock）
- `01_basic_rwlock.c` - 基本读写锁示例
- `02_cache_consistency.c` - 缓存一致性示例

### 4. 自旋锁（Spin Lock）
- `01_basic_spinlock.c` - 基本自旋锁示例
- `02_spinlock_vs_mutex.c` - 自旋锁与互斥锁性能比较

### 5. 屏障（Barrier）
- `01_basic_barrier.c` - 基本屏障示例
- `02_parallel_compute.c` - 多阶段并行计算示例

### 6. POSIX 信号量
- `01_unnamed_semaphore.c` - 未命名信号量示例
- `02_named_semaphore.c` - 命名信号量示例
- `03_resource_pool.c` - 资源池管理示例

### 7. 线程特定信号处理
- `01_signal_mask.c` - 信号掩码示例
- `02_timer_signal.c` - 定时器信号示例

### 8. 性能比较
- `01_sync_performance.c` - 各种同步机制性能比较

### 9. 实际应用场景
- `01_thread_pool.c` - 线程池实现
- `02_parallel_sort.c` - 并行排序
- `03_parallel_matrix.c` - 并行矩阵乘法

### 10. 最佳实践
- `01_choose_sync_mechanism.c` - 选择合适的同步机制
- `02_avoid_deadlock.c` - 避免死锁
- `03_minimize_critical_section.c` - 最小化临界区

## 编译要求

所有示例都需要以下编译器和库：
- GCC 编译器
- pthread 库
- 某些示例需要 rt 库（实时扩展）

## 注意事项

1. 某些示例可能需要特定的 Linux 特性支持
2. 性能测试结果可能因硬件和系统负载而异
3. 某些示例（如死锁演示）可能需要手动终止
4. 建议在 Linux 系统上运行这些示例

## 相关文档

详细的同步机制说明请参考：`doc/linux_pthread_synchronization_complete_guide.md`