# sd-bus 数据类型详解

## 目录
1. [概述](#概述)
2. [基本数据类型](#基本数据类型)
3. [容器类型](#容器类型)
4. [类型签名](#类型签名)
5. [完整示例](#完整示例)
6. [最佳实践](#最佳实践)

---

## 概述

sd-bus 完全支持 D-Bus 协议的所有数据类型，包括基本类型和复杂的容器类型。D-Bus 使用类型签名（type signature）来描述数据类型，每个类型都有一个对应的单字符代码。

### 支持的数据类型总结

| 类型 | 代码 | 说明 | C 类型 |
|------|------|------|--------|
| **基本类型** | | | |
| BYTE | `y` | 无符号8位整数 | `uint8_t` |
| BOOLEAN | `b` | 布尔值 | `int` |
| INT16 | `n` | 有符号16位整数 | `int16_t` |
| UINT16 | `q` | 无符号16位整数 | `uint16_t` |
| INT32 | `i` | 有符号32位整数 | `int32_t` |
| UINT32 | `u` | 无符号32位整数 | `uint32_t` |
| INT64 | `x` | 有符号64位整数 | `int64_t` |
| UINT64 | `t` | 无符号64位整数 | `uint64_t` |
| DOUBLE | `d` | IEEE 754 双精度浮点数 | `double` |
| UNIX_FD | `h` | Unix 文件描述符 | `int` |
| STRING | `s` | 字符串 | `const char *` |
| OBJECT_PATH | `o` | 对象路径 | `const char *` |
| SIGNATURE | `g` | 类型签名 | `const char *` |
| **容器类型** | | | |
| ARRAY | `a` | 数组 | - |
| STRUCT | `(` `)` | 结构体 | - |
| VARIANT | `v` | 变体类型 | - |
| DICT_ENTRY | `{` `}` | 字典条目 | - |
| **特殊类型** | | | |
| INVALID | `?` | 无效类型 | - |

---

## 基本数据类型

### 1. 整数类型

#### BYTE (y) - 无符号8位整数

```c
#include <systemd/sd-bus.h>

// 发送 BYTE 类型
int send_byte(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        uint8_t value = 255;
        sd_bus_message_append(m, "y", value);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收 BYTE 类型
int receive_byte(sd_bus_message *m) {
    uint8_t value;
    int r = sd_bus_message_read(m, "y", &value);
    if (r >= 0) {
        printf("BYTE value: %u\n", value);
    }
    return r;
}
```

#### INT16 (n) - 有符号16位整数

```c
// 发送 INT16
int send_int16(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        int16_t value = -1000;
        sd_bus_message_append(m, "n", value);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收 INT16
int receive_int16(sd_bus_message *m) {
    int16_t value;
    int r = sd_bus_message_read(m, "n", &value);
    if (r >= 0) {
        printf("INT16 value: %d\n", value);
    }
    return r;
}
```

#### UINT16 (q) - 无符号16位整数

```c
// 发送 UINT16
int send_uint16(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        uint16_t value = 50000;
        sd_bus_message_append(m, "q", value);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}
```

#### INT32 (i) - 有符号32位整数

```c
// 发送 INT32
int send_int32(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        int32_t value = -1000000;
        sd_bus_message_append(m, "i", value);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}
```

#### UINT32 (u) - 无符号32位整数

```c
// 发送 UINT32
int send_uint32(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        uint32_t value = 4000000000;
        sd_bus_message_append(m, "u", value);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}
```

#### INT64 (x) - 有符号64位整数

```c
// 发送 INT64
int send_int64(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        int64_t value = -9000000000000000000LL;
        sd_bus_message_append(m, "x", value);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}
```

#### UINT64 (t) - 无符号64位整数

```c
// 发送 UINT64
int send_uint64(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        uint64_t value = 18000000000000000000ULL;
        sd_bus_message_append(m, "t", value);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}
```

### 2. 浮点类型

#### DOUBLE (d) - IEEE 754 双精度浮点数

```c
// 发送 DOUBLE
int send_double(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        double value = 3.141592653589793;
        sd_bus_message_append(m, "d", value);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收 DOUBLE
int receive_double(sd_bus_message *m) {
    double value;
    int r = sd_bus_message_read(m, "d", &value);
    if (r >= 0) {
        printf("DOUBLE value: %.15f\n", value);
    }
    return r;
}
```

### 3. 布尔类型

#### BOOLEAN (b) - 布尔值

```c
// 发送 BOOLEAN
int send_boolean(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        int value = 1;  // true
        sd_bus_message_append(m, "b", value);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收 BOOLEAN
int receive_boolean(sd_bus_message *m) {
    int value;
    int r = sd_bus_message_read(m, "b", &value);
    if (r >= 0) {
        printf("BOOLEAN value: %s\n", value ? "true" : "false");
    }
    return r;
}
```

### 4. 字符串类型

#### STRING (s) - 字符串

```c
// 发送 STRING
int send_string(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        const char *value = "Hello, D-Bus!";
        sd_bus_message_append(m, "s", value);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收 STRING
int receive_string(sd_bus_message *m) {
    const char *value;
    int r = sd_bus_message_read(m, "s", &value);
    if (r >= 0) {
        printf("STRING value: %s\n", value);
    }
    return r;
}
```

#### OBJECT_PATH (o) - 对象路径

```c
// 发送 OBJECT_PATH
int send_object_path(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        const char *path = "/com/example/MyObject";
        sd_bus_message_append(m, "o", path);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收 OBJECT_PATH
int receive_object_path(sd_bus_message *m) {
    const char *path;
    int r = sd_bus_message_read(m, "o", &path);
    if (r >= 0) {
        printf("OBJECT_PATH: %s\n", path);
    }
    return r;
}
```

#### SIGNATURE (g) - 类型签名

```c
// 发送 SIGNATURE
int send_signature(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        const char *sig = "a{sv}";
        sd_bus_message_append(m, "g", sig);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收 SIGNATURE
int receive_signature(sd_bus_message *m) {
    const char *sig;
    int r = sd_bus_message_read(m, "g", &sig);
    if (r >= 0) {
        printf("SIGNATURE: %s\n", sig);
    }
    return r;
}
```

### 5. 文件描述符类型

#### UNIX_FD (h) - Unix 文件描述符

```c
#include <fcntl.h>

// 发送 UNIX_FD
int send_unix_fd(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        int fd = open("/tmp/test.txt", O_RDONLY);
        if (fd >= 0) {
            sd_bus_message_append(m, "h", fd);
            sd_bus_send(bus, m, NULL);
            close(fd);  // sd-bus 会复制文件描述符
            sd_bus_message_unref(m);
        }
    }

    return r;
}

// 接收 UNIX_FD
int receive_unix_fd(sd_bus_message *m) {
    int fd;
    int r = sd_bus_message_read(m, "h", &fd);
    if (r >= 0) {
        printf("Received file descriptor: %d\n", fd);
        // 使用文件描述符
        // ...
        close(fd);  // 使用后关闭
    }
    return r;
}
```

---

## 容器类型

### 1. ARRAY (a) - 数组

#### 基本数组

```c
// 发送整数数组
int send_int_array(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        // 打开数组容器
        r = sd_bus_message_open_container(m, 'a', "i");
        if (r < 0) {
            sd_bus_message_unref(m);
            return r;
        }

        // 添加数组元素
        int32_t values[] = {10, 20, 30, 40, 50};
        for (size_t i = 0; i < 5; i++) {
            sd_bus_message_append(m, "i", values[i]);
        }

        // 关闭数组容器
        sd_bus_message_close_container(m);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收整数数组
int receive_int_array(sd_bus_message *m) {
    int r;

    // 进入数组容器
    r = sd_bus_message_enter_container(m, 'a', "i");
    if (r < 0) return r;

    printf("Array elements:\n");
    while ((r = sd_bus_message_read(m, "i", NULL)) > 0) {
        int32_t value;
        sd_bus_message_read(m, "i", &value);
        printf("  %d\n", value);
    }

    // 退出数组容器
    sd_bus_message_exit_container(m);
    return 0;
}
```

#### 字符串数组

```c
// 发送字符串数组
int send_string_array(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        // 打开字符串数组容器
        r = sd_bus_message_open_container(m, 'a', "s");
        if (r < 0) {
            sd_bus_message_unref(m);
            return r;
        }

        // 添加字符串元素
        const char *strings[] = {"apple", "banana", "cherry", "date"};
        for (size_t i = 0; i < 4; i++) {
            sd_bus_message_append(m, "s", strings[i]);
        }

        // 关闭数组容器
        sd_bus_message_close_container(m);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收字符串数组
int receive_string_array(sd_bus_message *m) {
    int r;

    // 进入字符串数组容器
    r = sd_bus_message_enter_container(m, 'a', "s");
    if (r < 0) return r;

    printf("String array:\n");
    while ((r = sd_bus_message_read(m, "s", NULL)) > 0) {
        const char *value;
        sd_bus_message_read(m, "s", &value);
        printf("  %s\n", value);
    }

    // 退出数组容器
    sd_bus_message_exit_container(m);
    return 0;
}
```

#### 字节数组

```c
// 发送字节数组
int send_byte_array(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        // 打开字节数组容器
        r = sd_bus_message_open_container(m, 'a', "y");
        if (r < 0) {
            sd_bus_message_unref(m);
            return r;
        }

        // 添加字节元素
        uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
        for (size_t i = 0; i < 5; i++) {
            sd_bus_message_append(m, "y", data[i]);
        }

        // 关闭数组容器
        sd_bus_message_close_container(m);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}
```

### 2. STRUCT (()) - 结构体

#### 简单结构体

```c
// 发送结构体
int send_struct(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        // 打开结构体容器 (包含 int32, string, double)
        r = sd_bus_message_open_container(m, 'r', "isd");
        if (r < 0) {
            sd_bus_message_unref(m);
            return r;
        }

        // 添加结构体字段
        sd_bus_message_append(m, "i", 42);
        sd_bus_message_append(m, "s", "Hello");
        sd_bus_message_append(m, "d", 3.14);

        // 关闭结构体容器
        sd_bus_message_close_container(m);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收结构体
int receive_struct(sd_bus_message *m) {
    int r;

    // 进入结构体容器
    r = sd_bus_message_enter_container(m, 'r', "isd");
    if (r < 0) return r;

    // 读取结构体字段
    int32_t int_val;
    const char *str_val;
    double dbl_val;

    sd_bus_message_read(m, "i", &int_val);
    sd_bus_message_read(m, "s", &str_val);
    sd_bus_message_read(m, "d", &dbl_val);

    printf("Struct: int=%d, string=%s, double=%.2f\n", 
           int_val, str_val, dbl_val);

    // 退出结构体容器
    sd_bus_message_exit_container(m);
    return 0;
}
```

#### 嵌套结构体

```c
// 发送嵌套结构体
int send_nested_struct(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        // 打开外层结构体 (包含 int, struct)
        r = sd_bus_message_open_container(m, 'r', "i(is)");
        if (r < 0) {
            sd_bus_message_unref(m);
            return r;
        }

        // 添加外层字段
        sd_bus_message_append(m, "i", 100);

        // 打开内层结构体
        sd_bus_message_open_container(m, 'r', "is");
        sd_bus_message_append(m, "i", 200);
        sd_bus_message_append(m, "s", "nested");
        sd_bus_message_close_container(m);

        // 关闭外层结构体
        sd_bus_message_close_container(m);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收嵌套结构体
int receive_nested_struct(sd_bus_message *m) {
    int r;

    // 进入外层结构体
    r = sd_bus_message_enter_container(m, 'r', "i(is)");
    if (r < 0) return r;

    // 读取外层字段
    int32_t outer_int;
    sd_bus_message_read(m, "i", &outer_int);

    // 进入内层结构体
    sd_bus_message_enter_container(m, 'r', "is");
    int32_t inner_int;
    const char *inner_str;
    sd_bus_message_read(m, "i", &inner_int);
    sd_bus_message_read(m, "s", &inner_str);
    sd_bus_message_exit_container(m);

    printf("Nested struct: outer=%d, inner=(%d, %s)\n",
           outer_int, inner_int, inner_str);

    // 退出外层结构体
    sd_bus_message_exit_container(m);
    return 0;
}
```

### 3. VARIANT (v) - 变体类型

#### 发送和接收变体

```c
// 发送变体类型
int send_variant(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        // 发送字符串变体
        sd_bus_message_append(m, "v", "s", "Hello, Variant!");

        // 发送整数变体
        sd_bus_message_append(m, "v", "i", 42);

        // 发送数组变体
        sd_bus_message_open_container(m, 'v', "ai");
        sd_bus_message_open_container(m, 'a', "i");
        sd_bus_message_append(m, "i", 1);
        sd_bus_message_append(m, "i", 2);
        sd_bus_message_append(m, "i", 3);
        sd_bus_message_close_container(m);
        sd_bus_message_close_container(m);

        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收变体类型
int receive_variant(sd_bus_message *m) {
    int r;

    // 读取变体
    char type;
    const char *contents;

    // 查看变体类型
    r = sd_bus_message_peek_type(m, &type, &contents);
    if (r < 0) return r;

    printf("Variant type: %c, contents: %s\n", type, contents);

    // 根据类型读取值
    if (type == 's') {
        const char *value;
        sd_bus_message_read(m, "v", "s", &value);
        printf("String variant: %s\n", value);
    } else if (type == 'i') {
        int32_t value;
        sd_bus_message_read(m, "v", "i", &value);
        printf("Int variant: %d\n", value);
    } else if (type == 'a') {
        // 处理数组变体
        sd_bus_message_enter_container(m, 'v', contents);
        sd_bus_message_enter_container(m, 'a', contents + 1);
        
        printf("Array variant: [");
        while (sd_bus_message_read(m, contents + 1, NULL) > 0) {
            int32_t value;
            sd_bus_message_read(m, contents + 1, &value);
            printf("%d ", value);
        }
        printf("]\n");
        
        sd_bus_message_exit_container(m);
        sd_bus_message_exit_container(m);
    }

    return 0;
}
```

### 4. DICT_ENTRY ({}) - 字典条目

#### 字典（字典条目数组）

```c
// 发送字典
int send_dict(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        // 打开字典数组容器 (string -> int32)
        r = sd_bus_message_open_container(m, 'a', "{si}");
        if (r < 0) {
            sd_bus_message_unref(m);
            return r;
        }

        // 添加字典条目
        struct {
            const char *key;
            int32_t value;
        } entries[] = {
            {"apple", 10},
            {"banana", 20},
            {"cherry", 30}
        };

        for (size_t i = 0; i < 3; i++) {
            // 打开字典条目
            sd_bus_message_open_container(m, 'e', "si");
            sd_bus_message_append(m, "s", entries[i].key);
            sd_bus_message_append(m, "i", entries[i].value);
            sd_bus_message_close_container(m);
        }

        // 关闭字典数组容器
        sd_bus_message_close_container(m);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收字典
int receive_dict(sd_bus_message *m) {
    int r;

    // 进入字典数组容器
    r = sd_bus_message_enter_container(m, 'a', "{si}");
    if (r < 0) return r;

    printf("Dictionary:\n");
    while ((r = sd_bus_message_enter_container(m, 'e', "si")) > 0) {
        const char *key;
        int32_t value;

        sd_bus_message_read(m, "s", &key);
        sd_bus_message_read(m, "i", &value);

        printf("  %s: %d\n", key, value);

        sd_bus_message_exit_container(m);
    }

    // 退出字典数组容器
    sd_bus_message_exit_container(m);
    return 0;
}
```

#### 复杂字典（字符串到变体）

```c
// 发送复杂字典 (string -> variant)
int send_complex_dict(sd_bus *bus) {
    sd_bus_message *m = NULL;
    int r;

    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.Service",
        "/com/example/Object",
        "com.example.Interface",
        "Method");

    if (r >= 0) {
        // 打开字典数组容器 (string -> variant)
        r = sd_bus_message_open_container(m, 'a', "{sv}");
        if (r < 0) {
            sd_bus_message_unref(m);
            return r;
        }

        // 添加字符串值
        sd_bus_message_open_container(m, 'e', "sv");
        sd_bus_message_append(m, "s", "name");
        sd_bus_message_append(m, "v", "s", "John Doe");
        sd_bus_message_close_container(m);

        // 添加整数值
        sd_bus_message_open_container(m, 'e', "sv");
        sd_bus_message_append(m, "s", "age");
        sd_bus_message_append(m, "v", "i", 30);
        sd_bus_message_close_container(m);

        // 添加布尔值
        sd_bus_message_open_container(m, 'e', "sv");
        sd_bus_message_append(m, "s", "active");
        sd_bus_message_append(m, "v", "b", 1);
        sd_bus_message_close_container(m);

        // 关闭字典数组容器
        sd_bus_message_close_container(m);
        sd_bus_send(bus, m, NULL);
        sd_bus_message_unref(m);
    }

    return r;
}

// 接收复杂字典
int receive_complex_dict(sd_bus_message *m) {
    int r;

    // 进入字典数组容器
    r = sd_bus_message_enter_container(m, 'a', "{sv}");
    if (r < 0) return r;

    printf("Complex dictionary:\n");
    while ((r = sd_bus_message_enter_container(m, 'e', "sv")) > 0) {
        const char *key;
        char type;
        const char *contents;

        sd_bus_message_read(m, "s", &key);
        sd_bus_message_peek_type(m, &type, &contents);

        printf("  %s: ", key);

        if (type == 's') {
            const char *value;
            sd_bus_message_read(m, "v", "s", &value);
            printf("%s\n", value);
        } else if (type == 'i') {
            int32_t value;
            sd_bus_message_read(m, "v", "i", &value);
            printf("%d\n", value);
        } else if (type == 'b') {
            int value;
            sd_bus_message_read(m, "v", "b", &value);
            printf("%s\n", value ? "true" : "false");
        }

        sd_bus_message_exit_container(m);
    }

    // 退出字典数组容器
    sd_bus_message_exit_container(m);
    return 0;
}
```

---

## 类型签名

### 类型签名语法

D-Bus 使用类型签名字符串来描述数据类型：

| 代码 | 类型 | 示例 |
|------|------|------|
| `y` | BYTE | `"y"` |
| `b` | BOOLEAN | `"b"` |
| `n` | INT16 | `"n"` |
| `q` | UINT16 | `"q"` |
| `i` | INT32 | `"i"` |
| `u` | UINT32 | `"u"` |
| `x` | INT64 | `"x"` |
| `t` | UINT64 | `"t"` |
| `d` | DOUBLE | `"d"` |
| `s` | STRING | `"s"` |
| `o` | OBJECT_PATH | `"o"` |
| `g` | SIGNATURE | `"g"` |
| `h` | UNIX_FD | `"h"` |
| `a` | ARRAY | `"ai"` (整数数组) |
| `(...)` | STRUCT | `"(is)"` (包含int和string的结构体) |
| `v` | VARIANT | `"v"` |
| `{...}` | DICT_ENTRY | `"{si}"` (string到int的字典条目) |

### 复杂类型签名示例

```c
// 整数数组
"ai"

// 字符串数组
"as"

// 结构体 (int, string, double)
"(isd)"

// 嵌套结构体 (int, (int, string))
"(i(is))"

// 字典 (string -> int)
"a{si}"

// 复杂字典 (string -> variant)
"a{sv}"

// 数组的数组
"aai"

// 结构体数组
"a(isd)"

// 复杂嵌套类型
"a{sa{sv}}"  // 字典的字典
```

---

## 完整示例

### 示例 1: 复杂数据类型服务端

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <systemd/sd-bus.h>

// 处理复杂数据类型的方法
static int method_process_complex(sd_bus_message *m, void *userdata, 
                                   sd_bus_error *ret_error) {
    int r;

    printf("=== Processing complex data ===\n");

    // 读取字符串
    const char *str_val;
    r = sd_bus_message_read(m, "s", &str_val);
    if (r < 0) return r;
    printf("String: %s\n", str_val);

    // 读取整数数组
    r = sd_bus_message_enter_container(m, 'a', "i");
    if (r < 0) return r;
    
    printf("Integer array: [");
    while ((r = sd_bus_message_read(m, "i", NULL)) > 0) {
        int32_t val;
        sd_bus_message_read(m, "i", &val);
        printf("%d ", val);
    }
    printf("]\n");
    sd_bus_message_exit_container(m);

    // 读取结构体
    r = sd_bus_message_enter_container(m, 'r', "isd");
    if (r < 0) return r;
    
    int32_t int_val;
    const char *struct_str;
    double dbl_val;
    
    sd_bus_message_read(m, "i", &int_val);
    sd_bus_message_read(m, "s", &struct_str);
    sd_bus_message_read(m, "d", &dbl_val);
    
    printf("Struct: int=%d, string=%s, double=%.2f\n", 
           int_val, struct_str, dbl_val);
    sd_bus_message_exit_container(m);

    // 读取字典
    r = sd_bus_message_enter_container(m, 'a', "{sv}");
    if (r < 0) return r;
    
    printf("Dictionary:\n");
    while ((r = sd_bus_message_enter_container(m, 'e', "sv")) > 0) {
        const char *key;
        char type;
        const char *contents;
        
        sd_bus_message_read(m, "s", &key);
        sd_bus_message_peek_type(m, &type, &contents);
        
        printf("  %s: ", key);
        
        if (type == 's') {
            const char *value;
            sd_bus_message_read(m, "v", "s", &value);
            printf("%s\n", value);
        } else if (type == 'i') {
            int32_t value;
            sd_bus_message_read(m, "v", "i", &value);
            printf("%d\n", value);
        } else if (type == 'b') {
            int value;
            sd_bus_message_read(m, "v", "b", &value);
            printf("%s\n", value ? "true" : "false");
        }
        
        sd_bus_message_exit_container(m);
    }
    sd_bus_message_exit_container(m);

    // 返回成功
    return sd_bus_reply_method_return(m, "s", "Data processed successfully");
}

// vtable 定义
static const sd_bus_vtable complex_vtable[] = {
    SD_BUS_VTABLE_START(0),
    SD_BUS_METHOD("ProcessComplex", "sa(isd)a{sv}", "s", 
                  method_process_complex, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_VTABLE_END
};

int main(int argc, char *argv[]) {
    sd_bus_slot *slot = NULL;
    sd_bus *bus = NULL;
    int r;

    // 连接到会话总线
    r = sd_bus_default_user(&bus);
    if (r < 0) {
        fprintf(stderr, "Failed to connect to user bus: %s\n", strerror(-r));
        goto finish;
    }

    // 注册对象路径
    r = sd_bus_add_object_vtable(
        bus,
        &slot,
        "/com/example/ComplexTypes",
        "com.example.ComplexTypes",
        complex_vtable,
        NULL
    );

    if (r < 0) {
        fprintf(stderr, "Failed to register object: %s\n", strerror(-r));
        goto finish;
    }

    // 请求服务名
    r = sd_bus_request_name(bus, "com.example.ComplexTypes", 0);
    if (r < 0) {
        fprintf(stderr, "Failed to request name: %s\n", strerror(-r));
        goto finish;
    }

    printf("Complex types service started\n");
    printf("Listening on: /com/example/ComplexTypes\n");
    printf("Service name: com.example.ComplexTypes\n");

    // 事件循环
    for (;;) {
        r = sd_bus_process(bus, NULL);
        if (r < 0) {
            fprintf(stderr, "Failed to process bus: %s\n", strerror(-r));
            break;
        }

        if (r == 0) {
            r = sd_bus_wait(bus, (uint64_t)-1);
            if (r < 0) {
                fprintf(stderr, "Failed to wait for bus: %s\n", strerror(-r));
                break;
            }
        }
    }

finish:
    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);

    return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
```

### 示例 2: 复杂数据类型客户端

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <systemd/sd-bus.h>

int main(int argc, char *argv[]) {
    sd_bus *bus = NULL;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;
    sd_bus_message *reply = NULL;
    int r;

    // 连接到会话总线
    r = sd_bus_default_user(&bus);
    if (r < 0) {
        fprintf(stderr, "Failed to connect to user bus: %s\n", strerror(-r));
        goto finish;
    }

    // 创建方法调用
    r = sd_bus_message_new_method_call(bus, &m,
        "com.example.ComplexTypes",
        "/com/example/ComplexTypes",
        "com.example.ComplexTypes",
        "ProcessComplex");

    if (r < 0) {
        fprintf(stderr, "Failed to create method call: %s\n", strerror(-r));
        goto finish;
    }

    // 添加字符串
    sd_bus_message_append(m, "s", "Hello, Complex Types!");

    // 添加整数数组
    sd_bus_message_open_container(m, 'a', "i");
    int32_t int_array[] = {10, 20, 30, 40, 50};
    for (size_t i = 0; i < 5; i++) {
        sd_bus_message_append(m, "i", int_array[i]);
    }
    sd_bus_message_close_container(m);

    // 添加结构体
    sd_bus_message_open_container(m, 'r', "isd");
    sd_bus_message_append(m, "i", 42);
    sd_bus_message_append(m, "s", "Struct Data");
    sd_bus_message_append(m, "d", 3.14159);
    sd_bus_message_close_container(m);

    // 添加字典
    sd_bus_message_open_container(m, 'a', "{sv}");
    
    // 字符串值
    sd_bus_message_open_container(m, 'e', "sv");
    sd_bus_message_append(m, "s", "name");
    sd_bus_message_append(m, "v", "s", "John Doe");
    sd_bus_message_close_container(m);
    
    // 整数值
    sd_bus_message_open_container(m, 'e', "sv");
    sd_bus_message_append(m, "s", "age");
    sd_bus_message_append(m, "v", "i", 30);
    sd_bus_message_close_container(m);
    
    // 布尔值
    sd_bus_message_open_container(m, 'e', "sv");
    sd_bus_message_append(m, "s", "active");
    sd_bus_message_append(m, "v", "b", 1);
    sd_bus_message_close_container(m);
    
    sd_bus_message_close_container(m);

    // 发送消息
    r = sd_bus_send_with_reply_and_block(bus, m, 0, &error, &reply);
    if (r < 0) {
        fprintf(stderr, "Failed to call method: %s\n", error.message);
        goto finish;
    }

    // 读取返回值
    const char *result;
    r = sd_bus_message_read(reply, "s", &result);
    if (r >= 0) {
        printf("Result: %s\n", result);
    }

finish:
    sd_bus_message_unref(m);
    sd_bus_message_unref(reply);
    sd_bus_error_free(&error);
    sd_bus_unref(bus);

    return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
```

### 示例 3: 使用 busctl 测试复杂数据类型

```bash
# 启动服务端
./complex_server

# 在另一个终端使用 busctl 测试

# 1. 调用方法（简单参数）
busctl --user call com.example.ComplexTypes \
    /com/example/ComplexTypes \
    com.example.ComplexTypes \
    ProcessComplex \
    "sa(isd)a{sv}" \
    "Test" \
    3 1 2 3 \
    42 "Struct" 3.14 \
    2 "name" "s" "John" "age" "i" 30

# 2. 查看接口
busctl --user introspect com.example.ComplexTypes /com/example/ComplexTypes

# 3. 监控信号
busctl --user monitor com.example.ComplexTypes
```

---

## 最佳实践

### 1. 类型安全

```c
// 始终检查类型签名
int safe_read(sd_bus_message *m) {
    char type;
    const char *contents;
    int r;

    // 查看下一个类型
    r = sd_bus_message_peek_type(m, &type, &contents);
    if (r < 0) {
        fprintf(stderr, "Failed to peek type\n");
        return r;
    }

    // 根据类型处理
    switch (type) {
        case 's': {
            const char *value;
            r = sd_bus_message_read(m, "s", &value);
            if (r >= 0) printf("String: %s\n", value);
            break;
        }
        case 'i': {
            int32_t value;
            r = sd_bus_message_read(m, "i", &value);
            if (r >= 0) printf("Int: %d\n", value);
            break;
        }
        default:
            fprintf(stderr, "Unexpected type: %c\n", type);
            return -EINVAL;
    }

    return r;
}
```

### 2. 容器嵌套

```c
// 正确处理嵌套容器
int handle_nested(sd_bus_message *m) {
    int r;

    // 进入外层数组
    r = sd_bus_message_enter_container(m, 'a', "(is)");
    if (r < 0) return r;

    while ((r = sd_bus_message_enter_container(m, 'r', "is")) > 0) {
        int32_t int_val;
        const char *str_val;

        sd_bus_message_read(m, "i", &int_val);
        sd_bus_message_read(m, "s", &str_val);

        printf("Nested: %d, %s\n", int_val, str_val);

        sd_bus_message_exit_container(m);
    }

    sd_bus_message_exit_container(m);
    return 0;
}
```

### 3. 错误处理

```c
// 完善的错误处理
int safe_append(sd_bus_message *m, const char *type, ...) {
    va_list ap;
    int r;

    va_start(ap, type);
    r = sd_bus_message_appendv(m, type, ap);
    va_end(ap);

    if (r < 0) {
        fprintf(stderr, "Failed to append type '%s': %s\n", 
                type, strerror(-r));
        return r;
    }

    return 0;
}
```

### 4. 内存管理

```c
// 正确管理容器生命周期
int process_container(sd_bus_message *m) {
    int r;

    // 进入容器
    r = sd_bus_message_enter_container(m, 'a', "i");
    if (r < 0) return r;

    // 处理内容
    while ((r = sd_bus_message_read(m, "i", NULL)) > 0) {
        int32_t value;
        sd_bus_message_read(m, "i", &value);
        // 处理值
    }

    // 确保退出容器
    if (r >= 0) {
        r = sd_bus_message_exit_container(m);
    }

    return r;
}
```

### 5. 性能优化

```c
// 批量处理数组元素
int process_array_fast(sd_bus_message *m) {
    int r;

    r = sd_bus_message_enter_container(m, 'a', "i");
    if (r < 0) return r;

    // 使用循环批量处理
    while (sd_bus_message_read(m, "i", NULL) > 0) {
        int32_t value;
        sd_bus_message_read(m, "i", &value);
        // 快速处理
    }

    sd_bus_message_exit_container(m);
    return 0;
}
```

---

## 总结

sd-bus 完全支持 D-Bus 协议的所有数据类型，包括：

### 基本类型
- 整数类型：BYTE, INT16, UINT16, INT32, UINT32, INT64, UINT64
- 浮点类型：DOUBLE
- 布尔类型：BOOLEAN
- 字符串类型：STRING, OBJECT_PATH, SIGNATURE
- 文件描述符：UNIX_FD

### 容器类型
- **数组 (ARRAY)**：支持任意类型的数组，包括嵌套数组
- **结构体 (STRUCT)**：支持任意字段组合，包括嵌套结构体
- **变体 (VARIANT)**：可以包含任意类型的数据
- **字典 (DICT_ENTRY)**：键值对集合，通常作为字典条目数组使用

### 关键要点
1. 使用类型签名字符串描述数据类型
2. 容器类型需要正确地打开和关闭
3. 嵌套容器需要按顺序进入和退出
4. 始终检查返回值和类型
5. 使用 `sd_bus_message_peek_type()` 查看下一个类型
6. 变体类型需要动态处理

通过合理使用这些数据类型，可以构建功能强大且类型安全的 D-Bus 应用程序。