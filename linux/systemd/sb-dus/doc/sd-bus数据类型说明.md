# sd-bus 数据类型完整指南

## 概述

sd-bus 是 systemd 提供的 D-Bus IPC（进程间通信）高级封装 API。D-Bus 使用类型签名系统来定义消息中数据的类型，sd-bus 完全支持 D-Bus 类型系统。

**核心答案：sd-bus 完全支持结构体、数组和字典（字典数组）。**

---

## 一、基本数据类型（Basic Types）

| 类型签名 | C 类型 | 说明 | 字节数 |
|---------|--------|------|--------|
| `y` | `uint8_t` | 无符号字节 | 1 |
| `b` | `int` | 布尔值 | 4 |
| `n` | `int16_t` | 有符号 16 位整数 | 2 |
| `q` | `uint16_t` | 无符号 16 位整数 | 2 |
| `i` | `int32_t` | 有符号 32 位整数 | 4 |
| `u` | `uint32_t` | 无符号 32 位整数 | 4 |
| `x` | `int64_t` | 有符号 64 位整数 | 8 |
| `t` | `uint64_t` | 无符号 64 位整数 | 8 |
| `d` | `double` | IEEE 754 双精度浮点数 | 8 |
| `s` | `char *` | 字符串 | 可变 |
| `o` | `char *` | 对象路径（字符串） | 可变 |
| `g` | `char *` | 类型签名（字符串） | 可变 |

### 基本类型使用示例

```c
// 读取基本类型
int r;
const char *str;
uint32_t value;
int64_t large_num;

r = sd_bus_message_read(m, "suix", &str, &value, &large_num);

// 追加基本类型
r = sd_bus_message_append(m, "sb", "hello", 1);
```

---

## 二、容器类型（Container Types）

### 1. 数组（Array）

类型签名：`a<元素类型>`

**数组是最常用的容器类型，可以包含任何基本类型或嵌套容器。**

#### 数组操作函数

```c
// 打开数组容器
int sd_bus_message_open_container(sd_bus_message *m, char type, const char *contents);

// 关闭容器
int sd_bus_message_close_container(sd_bus_message *m);

// 高效追加数组（已知数组指针和大小）
int sd_bus_message_append_array(sd_bus_message *m, char type, const void *ptr, size_t size);

// 读取数组
int sd_bus_message_read_array(sd_bus_message *m, char type, const void **ptr, size_t *size);
```

#### 数组示例

```c
// 方法1：逐个元素追加（适合复杂类型）
sd_bus_message_open_container(m, 'a', "i");  // 打开 int 数组
for (int i = 0; i < 10; i++) {
    sd_bus_message_append(m, "i", i);
}
sd_bus_message_close_container(m);  // 关闭数组

// 方法2：批量追加（适合基本类型数组）
int values[] = {1, 2, 3, 4, 5};
sd_bus_message_append_array(m, 'i', values, sizeof(values));

// 读取数组
const int32_t *array;
size_t size;
sd_bus_message_read_array(m, 'i', (const void**)&array, &size);
```

#### 多维数组示例

```c
// 二维数组：int[3][4] -> 类型签名 "aai"
sd_bus_message_open_container(m, 'a', "ai");  // 外层数组
for (int i = 0; i < 3; i++) {
    sd_bus_message_open_container(m, 'a', "i");  // 内层数组
    for (int j = 0; j < 4; j++) {
        sd_bus_message_append(m, "i", i * 4 + j);
    }
    sd_bus_message_close_container(m);
}
sd_bus_message_close_container(m);
```

---

### 2. 结构体（Struct）

类型签名：`(<类型1><类型2>...)`

**结构体是固定类型的字段序列，字段数量和类型固定。**

#### 结构体操作函数

```c
// 打开结构体容器
int sd_bus_message_open_container(sd_bus_message *m, 'r', const char *contents);
// 注意：D-Bus 使用 'r' 表示结构体（record）
```

#### 结构体示例

```c
// 定义结构体：(is) - int + string
sd_bus_message_open_container(m, 'r', "is");  // 打开结构体
sd_bus_message_append(m, "i", 42);            // 字段1：整数
sd_bus_message_append(m, "s", "answer");      // 字段2：字符串
sd_bus_message_close_container(m);            // 关闭结构体

// 嵌套结构体：(i(is))
sd_bus_message_open_container(m, 'r', "i(is)");
sd_bus_message_append(m, "i", 1);
sd_bus_message_open_container(m, 'r', "is");
sd_bus_message_append(m, "i", 2);
sd_bus_message_append(m, "s", "nested");
sd_bus_message_close_container(m);
sd_bus_message_close_container(m);

// 读取结构体
int num;
const char *str;
sd_bus_message_read(m, "(is)", &num, &str);
```

---

### 3. 字典（Dictionary Entry）

类型签名：`{<键类型><值类型>}`

**重要：字典不是独立类型，而是字典数组（a{...}）的特殊元素。**

#### 字典数组示例

```c
// 字典数组：a{ss} - 字符串到字符串的映射
// 相当于: map<string, string>

sd_bus_message_open_container(m, 'a', "{ss}");  // 打开字典数组

// 添加第一个键值对
sd_bus_message_open_container(m, 'e', "ss");    // 'e' = entry（字典条目）
sd_bus_message_append(m, "s", "name");
sd_bus_message_append(m, "s", "calculator");
sd_bus_message_close_container(m);

// 添加第二个键值对
sd_bus_message_open_container(m, 'e', "ss");
sd_bus_message_append(m, "s", "version");
sd_bus_message_append(m, "s", "1.0");
sd_bus_message_close_container(m);

sd_bus_message_close_container(m);  // 关闭字典数组

// 读取字典数组
sd_bus_message_enter_container(m, 'a', "{ss}");
while (true) {
    const char *key, *value;
    r = sd_bus_message_enter_container(m, 'e', "ss");
    if (r <= 0) break;  // 0 = 数组结束

    sd_bus_message_read(m, "ss", &key, &value);
    printf("%s: %s\n", key, value);

    sd_bus_message_exit_container(m);
}
sd_bus_message_exit_container(m);
```

#### 复杂字典类型

```c
// a{sv} - 字符串到任意类型的字典（最常用的字典类型）
// 相当于: map<string, variant>

sd_bus_message_open_container(m, 'a', "{sv}");

// 键值对 1: "count" -> int32
sd_bus_message_open_container(m, 'e', "sv");
sd_bus_message_append(m, "s", "count");
sd_bus_message_open_container(m, 'v', "i");
sd_bus_message_append(m, "i", 42);
sd_bus_message_close_container(m);
sd_bus_message_close_container(m);

// 键值对 2: "name" -> string
sd_bus_message_open_container(m, 'e', "sv");
sd_bus_message_append(m, "s", "name");
sd_bus_message_open_container(m, 'v', "s");
sd_bus_message_append(m, "s", "test");
sd_bus_message_close_container(m);
sd_bus_message_close_container(m);

sd_bus_message_close_container(m);
```

---

### 4. 变体类型（Variant）

类型签名：`v`

**变体可以包含任何单一类型，运行时决定具体类型。**

#### 变体示例

```c
// 创建变体
sd_bus_message_open_container(m, 'v', "s");  // 变体包含字符串
sd_bus_message_append(m, "s", "hello");
sd_bus_message_close_container(m);

// 变体包含结构体
sd_bus_message_open_container(m, 'v', "(is)");
sd_bus_message_open_container(m, 'r', "is");
sd_bus_message_append(m, "i", 123);
sd_bus_message_append(m, "s", "variant struct");
sd_bus_message_close_container(m);
sd_bus_message_close_container(m);

// 读取变体
const char *contents;
sd_bus_message_enter_container(m, 'v', &contents);
// 根据 contents 动态处理不同类型
if (strcmp(contents, "s") == 0) {
    const char *str;
    sd_bus_message_read(m, "s", &str);
}
sd_bus_message_exit_container(m);
```

---

## 三、特殊类型

### Unix FD（Unix 文件描述符）

类型签名：`h`

```c
// 发送文件描述符
int fd = socket(AF_UNIX, SOCK_STREAM, 0);
sd_bus_message_append(m, "h", fd);

// 接收文件描述符
int received_fd;
sd_bus_message_read(m, "h", &received_fd);
```

---

## 四、完整示例：复杂数据结构

### 示例1：vtable 方法定义

```c
// 方法返回复杂数据结构
static int method_get_complex(sd_bus_message *m, void *userdata,
                              sd_bus_error *ret_error) {
    sd_bus_message *reply = NULL;

    // 构建复杂响应
    // 签名: a{sa{sv}} - 字典数组（键为string，值为字典{string, variant}）
    sd_bus_message_new_method_return(m, &reply);

    sd_bus_message_open_container(reply, 'a', "{sa{sv}}");

    // 第一组数据
    sd_bus_message_open_container(reply, 'e', "sa{sv}");
    sd_bus_message_append(reply, "s", "item1");

    sd_bus_message_open_container(reply, 'a', "{sv}");
    // 添加属性
    sd_bus_message_open_container(reply, 'e', "sv");
    sd_bus_message_append(reply, "s", "value");
    sd_bus_message_open_container(reply, 'v', "i");
    sd_bus_message_append(reply, "i", 100);
    sd_bus_message_close_container(reply);  // v
    sd_bus_message_close_container(reply);  // entry
    sd_bus_message_close_container(reply);  // a{sv}
    sd_bus_message_close_container(reply);  // entry

    sd_bus_message_close_container(reply);  // a{sa{sv}}

    sd_bus_send(NULL, reply, NULL);
    sd_bus_message_unref(reply);

    return 0;
}

static const sd_bus_vtable example_vtable[] = {
    SD_BUS_VTABLE_START(0),
    SD_BUS_METHOD("GetComplex", NULL, "a{sa{sv}}", method_get_complex,
                  SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_VTABLE_END
};
```

### 示例2：读取复杂数据

```c
static int process_complex_data(sd_bus_message *m) {
    int r;

    // 读取字典数组: a{ia{sv}}
    r = sd_bus_message_enter_container(m, 'a', "{ia{sv}}");
    if (r < 0) return r;

    while (true) {
        r = sd_bus_message_enter_container(m, 'e', "ia{sv}");
        if (r <= 0) break;  // 0 = 数组结束

        int32_t key;
        sd_bus_message_read(m, "i", &key);

        printf("Key: %d\n", key);

        r = sd_bus_message_enter_container(m, 'a', "{sv}");
        if (r < 0) return r;

        while (true) {
            r = sd_bus_message_enter_container(m, 'e', "sv");
            if (r <= 0) break;

            const char *prop_name;
            const char *prop_type;

            sd_bus_message_read(m, "s", &prop_name);
            sd_bus_message_enter_container(m, 'v', &prop_type);

            printf("  Property: %s (type: %s)\n", prop_name, prop_type);

            if (strcmp(prop_type, "s") == 0) {
                const char *value;
                sd_bus_message_read(m, "s", &value);
                printf("    Value: %s\n", value);
            } else if (strcmp(prop_type, "i") == 0) {
                int32_t value;
                sd_bus_message_read(m, "i", &value);
                printf("    Value: %d\n", value);
            }

            sd_bus_message_exit_container(m);  // v
            sd_bus_message_exit_container(m);  // entry
        }

        sd_bus_message_exit_container(m);  // a{sv}
        sd_bus_message_exit_container(m);  // entry
    }

    sd_bus_message_exit_container(m);  // a{ia{sv}}

    return 0;
}
```

---

## 五、类型签名字符快速参考

| 字符 | 类型 | 容器类型 | 说明 |
|------|------|----------|------|
| `y` | BYTE | - | 字节 |
| `b` | BOOLEAN | - | 布尔值 |
| `n` | INT16 | - | 16位整数 |
| `q` | UINT16 | - | 16位无符号整数 |
| `i` | INT32 | - | 32位整数 |
| `u` | UINT32 | - | 32位无符号整数 |
| `x` | INT64 | - | 64位整数 |
| `t` | UINT64 | - | 64位无符号整数 |
| `d` | DOUBLE | - | 双精度浮点 |
| `s` | STRING | - | 字符串 |
| `o` | OBJECT_PATH | - | 对象路径 |
| `g` | SIGNATURE | - | 类型签名 |
| `h` | UNIX_FD | - | 文件描述符 |
| `a` | ARRAY | ✓ | 数组 |
| `r` / `(` | STRUCT | ✓ | 结构体 |
| `v` | VARIANT | ✓ | 变体类型 |
| `e` / `{` | DICT_ENTRY | ✓ | 字典条目（仅用于数组内） |
| `)` | - | ✓ | 结构体结束 |
| `}` | - | ✓ | 字典条目结束 |

---

## 六、最佳实践

### 1. 性能优化

```c
// ✓ 推荐：批量追加基本类型数组
int data[1000];
sd_bus_message_append_array(m, 'i', data, sizeof(data));

// ✗ 避免：逐个追加大数据集
for (int i = 0; i < 1000; i++) {
    sd_bus_message_append(m, "i", data[i]);
}
```

### 2. 错误处理

```c
#define CHECK_ERROR(call) \
    do { \
        int r = (call); \
        if (r < 0) { \
            fprintf(stderr, "Error %s: %s\n", #call, strerror(-r)); \
            return r; \
        } \
    } while(0)

CHECK_ERROR(sd_bus_message_append(m, "s", "hello"));
```

### 3. 容器配对检查

```c
// 确保每个 open_container 都有对应的 close_container
sd_bus_message_open_container(m, 'a', "i");
// ... 操作 ...
sd_bus_message_close_container(m);  // 不要忘记！
```

---

## 七、调试技巧

### 打印类型签名

```c
// 使用 gdb 或添加调试代码
printf("Type signature: %s\n", "a{sv}");
```

### 验证消息结构

```bash
# 使用 dbus-monitor 监控消息
dbus-monitor --session

# 使用 gdbus 工具测试
gdbus call --session --dest com.example.Calculator \
          --object-path /com/example/Calculator \
          --method com.example.Calculator.Echo "hello"
```

---

## 八、常见类型签名示例

| 签名 | 说明 | 示例 |
|------|------|------|
| `i` | 单个整数 | `42` |
| `s` | 单个字符串 | `"hello"` |
| `as` | 字符串数组 | `["a", "b", "c"]` |
| `ai` | 整数数组 | `[1, 2, 3]` |
| `(is)` | 整数+字符串结构体 | `(42, "answer")` |
| `a{ss}` | 字符串到字符串字典 | `{"key": "value"}` |
| `a{sv}` | 字符串到任意类型字典 | `{"name": "test", "count": 42}` |
| `aia{sv}` | 整数数组，后跟字典 | `([1, 2], {"key": "value"})` |
| `a(ii)` | 二维数组（整数对） | `[(1,2), (3,4)]` |

---

## 总结

**sd-bus 数据类型支持总结：**

✅ **完全支持的结构：**
- ✅ 基本类型（15种）
- ✅ 数组（单维和多维）
- ✅ 结构体（可嵌套）
- ✅ 字典（通过字典数组 `a{...}` 实现）
- ✅ 变体类型（动态类型）
- ✅ 文件描述符传递

**关键要点：**
1. **字典本质上是字典数组** `a{kt}`（k=键类型，t=值类型）
2. **结构体使用 `(types)` 或 `r` 表示**
3. **容器必须正确配对 open/close**
4. **类型签名是类型系统的核心**
5. **变体 `v` 提供动态类型能力**

**参考文档：**
- `man sd-bus`
- `man sd-bus-types`
- `man sd_bus_message_append`
- [D-Bus Specification](https://dbus.freedesktop.org/doc/dbus-specification.html)

---

*文档生成时间：2026-01-09*
*systemd 版本：254+*
