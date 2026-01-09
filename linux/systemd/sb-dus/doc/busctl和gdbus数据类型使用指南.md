# busctl 和 gdbus 数据类型使用指南

## 目录
1. [概述](#概述)
2. [busctl 数据类型使用](#busctl-数据类型使用)
3. [gdbus 数据类型使用](#gdbus-数据类型使用)
4. [对比与选择](#对比与选择)
5. [实用示例](#实用示例)
6. [最佳实践](#最佳实践)

---

## 概述

`busctl` 和 `gdbus` 是两个常用的 D-Bus 命令行工具，用于与 D-Bus 服务进行交互。它们都支持发送各种数据类型，但语法有所不同。

### 工具对比

| 特性 | busctl | gdbus |
|------|--------|-------|
| **来源** | systemd | GLib |
| **语法风格** | 简洁，类型签名在前 | 详细，参数名在前 |
| **JSON 支持** | 原生支持 | 不支持 |
| **输出格式** | 可配置（普通/JSON） | 固定格式 |
| **学习曲线** | 较陡 | 较平缓 |
| **适用场景** | 脚本、自动化 | 交互式调试 |

---

## busctl 数据类型使用

### 基本语法

```bash
busctl [OPTIONS] call NAME PATH INTERFACE METHOD SIGNATURE [ARGUMENTS...]
```

### 基本数据类型

#### 1. BYTE (y) - 无符号8位整数

```bash
# 发送 BYTE
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "y" 255

# 输出
y 255
```

#### 2. BOOLEAN (b) - 布尔值

```bash
# 发送 true
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "b" true

# 发送 false
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "b" false

# 输出
b true
```

#### 3. INT16 (n) - 有符号16位整数

```bash
# 发送 INT16
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "n" -1000

# 输出
n -1000
```

#### 4. UINT16 (q) - 无符号16位整数

```bash
# 发送 UINT16
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "q" 50000

# 输出
q 50000
```

#### 5. INT32 (i) - 有符号32位整数

```bash
# 发送 INT32
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "i" -1000000

# 输出
i -1000000
```

#### 6. UINT32 (u) - 无符号32位整数

```bash
# 发送 UINT32
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "u" 4000000000

# 输出
u 4000000000
```

#### 7. INT64 (x) - 有符号64位整数

```bash
# 发送 INT64
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "x" -9000000000000000000

# 输出
x -9000000000000000000
```

#### 8. UINT64 (t) - 无符号64位整数

```bash
# 发送 UINT64
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "t" 18000000000000000000

# 输出
t 18000000000000000000
```

#### 9. DOUBLE (d) - IEEE 754 双精度浮点数

```bash
# 发送 DOUBLE
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "d" 3.141592653589793

# 输出
d 3.141592653589793
```

#### 10. STRING (s) - 字符串

```bash
# 发送 STRING
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "s" "Hello, D-Bus!"

# 输出
s "Hello, D-Bus!"

# 发送包含空格的字符串
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "s" "Hello World"

# 发送包含特殊字符的字符串
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "s" "Line1\nLine2"
```

#### 11. OBJECT_PATH (o) - 对象路径

```bash
# 发送 OBJECT_PATH
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "o" "/com/example/MyObject"

# 输出
o "/com/example/MyObject"
```

#### 12. SIGNATURE (g) - 类型签名

```bash
# 发送 SIGNATURE
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "g" "a{sv}"

# 输出
g "a{sv}"
```

#### 13. UNIX_FD (h) - Unix 文件描述符

```bash
# 发送文件描述符（需要先打开文件）
exec 3<>/tmp/test.txt
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "h" 3
exec 3<&-

# 输出
h 42
```

### 容器类型

#### 1. ARRAY (a) - 数组

##### 整数数组

```bash
# 发送整数数组
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "ai" 10 20 30 40 50

# 输出
ai 5 10 20 30 40 50
```

##### 字符串数组

```bash
# 发送字符串数组
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "as" \
    "apple" "banana" "cherry" "date"

# 输出
as 4 "apple" "banana" "cherry" "date"
```

##### 字节数组

```bash
# 发送字节数组
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "ay" 1 2 3 4 5

# 输出
ay 5 1 2 3 4 5
```

##### 双精度浮点数组

```bash
# 发送双精度浮点数组
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "ad" 1.1 2.2 3.3 4.4 5.5

# 输出
ad 5 1.1 2.2 3.3 4.4 5.5
```

##### 布尔数组

```bash
# 发送布尔数组
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "ab" true false true false true

# 输出
ab 5 true false true false true
```

##### 嵌套数组（数组的数组）

```bash
# 发送数组的数组
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "aai" \
    2 1 2 3 2 4 5 6

# 解释：2个数组，第一个有3个元素(1,2,3)，第二个有3个元素(4,5,6)

# 输出
aai 2 3 1 2 3 3 4 5 6
```

#### 2. STRUCT (()) - 结构体

##### 简单结构体

```bash
# 发送结构体 (int, string, double)
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "(isd)" 42 "Hello" 3.14

# 输出
(isd) 42 "Hello" 3.14
```

##### 嵌套结构体

```bash
# 发送嵌套结构体 (int, (int, string))
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "(i(is))" 100 200 "nested"

# 输出
(i(is)) 100 (is) 200 "nested"
```

##### 复杂结构体

```bash
# 发送复杂结构体 (string, int, array of int)
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "(siai)" "test" 42 3 1 2 3

# 输出
(siai) "test" 42 ai 3 1 2 3
```

#### 3. VARIANT (v) - 变体类型

##### 字符串变体

```bash
# 发送字符串变体
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "v" "s" "Hello, Variant!"

# 输出
v s "Hello, Variant!"
```

##### 整数变体

```bash
# 发送整数变体
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "v" "i" 42

# 输出
v i 42
```

##### 数组变体

```bash
# 发送数组变体
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "v" "ai" 1 2 3 4 5

# 输出
v ai 5 1 2 3 4 5
```

##### 结构体变体

```bash
# 发送结构体变体
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "v" "(is)" 100 "test"

# 输出
v (is) 100 "test"
```

#### 4. DICT_ENTRY ({}) - 字典条目

##### 简单字典（string -> int）

```bash
# 发送字典 (string -> int)
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "a{si}" \
    3 "apple" 10 "banana" 20 "cherry" 30

# 输出
a{si} 3 "apple" 10 "banana" 20 "cherry" 30
```

##### 复杂字典（string -> variant）

```bash
# 发送复杂字典 (string -> variant)
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "a{sv}" \
    3 \
    "name" "s" "John Doe" \
    "age" "i" 30 \
    "active" "b" true

# 输出
a{sv} 3 "name" s "John Doe" "age" i 30 "active" b true
```

##### 字典（int -> string）

```bash
# 发送字典 (int -> string)
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "a{is}" \
    3 1 "one" 2 "two" 3 "three"

# 输出
a{is} 3 1 "one" 2 "two" 3 "three"
```

##### 嵌套字典

```bash
# 发送嵌套字典 (string -> dict)
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "a{sa{sv}}" \
    1 \
    "user1" 2 "name" "s" "Alice" "age" "i" 25

# 输出
a{sa{sv}} 1 "user1" a{sv} 2 "name" s "Alice" "age" i 25
```

### 复杂组合示例

#### 多个参数

```bash
# 发送多个不同类型的参数
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method "sia(is)a{sv}" \
    "Hello" 42 \
    100 200 "nested" \
    2 "key1" "s" "value1" "key2" "i" 123

# 输出
s "Hello" i 42 (i(is)) 100 (is) 200 "nested" a{sv} 2 "key1" s "value1" "key2" i 123
```

#### 使用 JSON 格式

```bash
# 使用 JSON 格式输出
busctl --user --json=pretty call com.example.Service /com/example/Object \
    com.example.Interface Method "a{sv}" \
    2 "name" "s" "John" "age" "i" 30

# 输出
{
  "type": "a{sv}",
  "data": [
    {
      "key": "name",
      "value": {
        "type": "s",
        "data": "John"
      }
    },
    {
      "key": "age",
      "value": {
        "type": "i",
        "data": 30
      }
    }
  ]
}
```

---

## gdbus 数据类型使用

### 基本语法

```bash
gdbus call --session \
    --dest NAME \
    --object-path PATH \
    --interface INTERFACE \
    --method METHOD \
    [ARGUMENTS...]
```

### 基本数据类型

#### 1. BYTE (y) - 无符号8位整数

```bash
# 发送 BYTE
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    255

# 输出
(uint8 255,)
```

#### 2. BOOLEAN (b) - 布尔值

```bash
# 发送 true
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    true

# 发送 false
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    false

# 输出
(boolean true,)
```

#### 3. INT16 (n) - 有符号16位整数

```bash
# 发送 INT16
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    -1000

# 输出
(int16 -1000,)
```

#### 4. UINT16 (q) - 无符号16位整数

```bash
# 发送 UINT16
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    50000

# 输出
(uint16 50000,)
```

#### 5. INT32 (i) - 有符号32位整数

```bash
# 发送 INT32
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    -1000000

# 输出
(int32 -1000000,)
```

#### 6. UINT32 (u) - 无符号32位整数

```bash
# 发送 UINT32
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    4000000000

# 输出
(uint32 4000000000,)
```

#### 7. INT64 (x) - 有符号64位整数

```bash
# 发送 INT64
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    -9000000000000000000

# 输出
(int64 -9000000000000000000,)
```

#### 8. UINT64 (t) - 无符号64位整数

```bash
# 发送 UINT64
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    18000000000000000000

# 输出
(uint64 18000000000000000000,)
```

#### 9. DOUBLE (d) - IEEE 754 双精度浮点数

```bash
# 发送 DOUBLE
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    3.141592653589793

# 输出
(double 3.141592653589793,)
```

#### 10. STRING (s) - 字符串

```bash
# 发送 STRING
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "Hello, D-Bus!"

# 输出
(string 'Hello, D-Bus!',)

# 发送包含空格的字符串
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "Hello World"
```

#### 11. OBJECT_PATH (o) - 对象路径

```bash
# 发送 OBJECT_PATH
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    objectpath:"/com/example/MyObject"

# 输出
(objectpath '/com/example/MyObject',)
```

#### 12. SIGNATURE (g) - 类型签名

```bash
# 发送 SIGNATURE
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    signature:"a{sv}"

# 输出
(signature 'a{sv}',)
```

#### 13. UNIX_FD (h) - Unix 文件描述符

```bash
# 发送文件描述符
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    handle:3

# 输出
(unixfd 42,)
```

### 容器类型

#### 1. ARRAY (a) - 数组

##### 整数数组

```bash
# 发送整数数组
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "[10, 20, 30, 40, 50]"

# 输出
(array of int32 [10, 20, 30, 40, 50],)
```

##### 字符串数组

```bash
# 发送字符串数组
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "['apple', 'banana', 'cherry', 'date']"

# 输出
(array of string ['apple', 'banana', 'cherry', 'date'],)
```

##### 字节数组

```bash
# 发送字节数组
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "bytearray:[1, 2, 3, 4, 5]"

# 输出
(array of byte [1, 2, 3, 4, 5],)
```

##### 双精度浮点数组

```bash
# 发送双精度浮点数组
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "[1.1, 2.2, 3.3, 4.4, 5.5]"

# 输出
(array of double [1.1, 2.2, 3.3, 4.4, 5.5],)
```

##### 布尔数组

```bash
# 发送布尔数组
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "[true, false, true, false, true]"

# 输出
(array of boolean [true, false, true, false, true],)
```

##### 嵌套数组（数组的数组）

```bash
# 发送数组的数组
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "[[1, 2, 3], [4, 5, 6]]"

# 输出
(array of array of int32 [[1, 2, 3], [4, 5, 6]],)
```

#### 2. STRUCT (()) - 结构体

##### 简单结构体

```bash
# 发送结构体 (int, string, double)
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "(42, 'Hello', 3.14)"

# 输出
((int32 42, string 'Hello', double 3.14),)
```

##### 嵌套结构体

```bash
# 发送嵌套结构体 (int, (int, string))
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "(100, (200, 'nested'))"

# 输出
((int32 100, (int32 200, string 'nested')),)
```

##### 复杂结构体

```bash
# 发送复杂结构体 (string, int, array of int)
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "('test', 42, [1, 2, 3])"

# 输出
((string 'test', int32 42, array of int32 [1, 2, 3]),)
```

#### 3. VARIANT (v) - 变体类型

##### 字符串变体

```bash
# 发送字符串变体
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "<'Hello, Variant!'>"

# 输出
(variant string 'Hello, Variant!',)
```

##### 整数变体

```bash
# 发送整数变体
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "<42>"

# 输出
(variant int32 42,)
```

##### 数组变体

```bash
# 发送数组变体
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "<[1, 2, 3, 4, 5]>"

# 输出
(variant array of int32 [1, 2, 3, 4, 5],)
```

##### 结构体变体

```bash
# 发送结构体变体
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "<(100, 'test')>"

# 输出
(variant (int32 100, string 'test'),)
```

#### 4. DICT_ENTRY ({}) - 字典条目

##### 简单字典（string -> int）

```bash
# 发送字典 (string -> int)
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "{'apple': 10, 'banana': 20, 'cherry': 30}"

# 输出
(array of dict entry(string, int32) [{'apple': 10}, {'banana': 20}, {'cherry': 30}],)
```

##### 复杂字典（string -> variant）

```bash
# 发送复杂字典 (string -> variant)
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "{'name': <'John Doe'>, 'age': <30>, 'active': <true>}"

# 输出
(array of dict entry(string, variant) [{'name': <'John Doe'>}, {'age': <30>}, {'active': <true>}],)
```

##### 字典（int -> string）

```bash
# 发送字典 (int -> string)
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "{1: 'one', 2: 'two', 3: 'three'}"

# 输出
(array of dict entry(int32, string) [{1: 'one'}, {2: 'two'}, {3: 'three'}],)
```

##### 嵌套字典

```bash
# 发送嵌套字典 (string -> dict)
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "{'user1': {'name': <'Alice'>, 'age': <25>}}"

# 输出
(array of dict entry(string, variant) [{'user1': <{'name': <'Alice'>, 'age': <25>}>}],)
```

### 复杂组合示例

#### 多个参数

```bash
# 发送多个不同类型的参数
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "'Hello' 42 (100, (200, 'nested')) {'key1': <'value1'>, 'key2': <123>}"

# 输出
(string 'Hello', int32 42, (int32 100, (int32 200, string 'nested')), array of dict entry(string, variant) [{'key1': <'value1'>}, {'key2': <123>}],)
```

---

## 对比与选择

### 语法对比表

| 数据类型 | busctl 语法 | gdbus 语法 |
|---------|------------|-----------|
| **整数** | `"i" 42` | `42` |
| **字符串** | `"s" "Hello"` | `'Hello'` |
| **布尔** | `"b" true` | `true` |
| **整数数组** | `"ai" 1 2 3` | `[1, 2, 3]` |
| **字符串数组** | `"as" "a" "b" "c"` | `['a', 'b', 'c']` |
| **结构体** | `"(is)" 42 "test"` | `(42, 'test')` |
| **变体** | `"v" "s" "test"` | `<'test'>` |
| **字典** | `"a{si}" 2 "a" 1 "b" 2` | `{'a': 1, 'b': 2}` |

### 选择建议

#### 使用 busctl 的场景
- 需要脚本自动化
- 需要精确控制类型签名
- 需要 JSON 输出格式
- 系统管理任务
- 性能敏感的场景

#### 使用 gdbus 的场景
- 交互式调试
- 需要更易读的语法
- 开发和测试
- 快速原型开发
- 学习 D-Bus

---

## 实用示例

### 示例 1: 使用 busctl 调用 systemd

```bash
# 列出所有单元
busctl --system call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    ListUnits

# 启动一个服务
busctl --system call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    StartUnit \
    "ss" "nginx.service" "replace"

# 获取单元属性
busctl --system get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/ssh_2eservice \
    org.freedesktop.systemd1.Unit \
    ActiveState
```

### 示例 2: 使用 gdbus 调用 systemd

```bash
# 列出所有单元
gdbus call --system \
    --dest org.freedesktop.systemd1 \
    --object-path /org/freedesktop/systemd1 \
    --interface org.freedesktop.systemd1.Manager \
    --method ListUnits

# 启动一个服务
gdbus call --system \
    --dest org.freedesktop.systemd1 \
    --object-path /org/freedesktop/systemd1 \
    --interface org.freedesktop.systemd1.Manager \
    --method StartUnit \
    "nginx.service" "replace"

# 获取单元属性
gdbus call --system \
    --dest org.freedesktop.systemd1 \
    --object-path /org/freedesktop/systemd1/unit/ssh_2eservice \
    --interface org.freedesktop.DBus.Properties \
    --method Get \
    "org.freedesktop.systemd1.Unit" "ActiveState"
```

### 示例 3: 发送桌面通知

#### 使用 busctl

```bash
busctl --user call org.freedesktop.Notifications \
    /org/freedesktop/Notifications \
    org.freedesktop.Notifications \
    Notify \
    "susssasa{sv}i" \
    "MyApp" 0 "dialog-information" \
    "Hello" "This is a notification" \
    "" "" "" -1
```

#### 使用 gdbus

```bash
gdbus call --session \
    --dest org.freedesktop.Notifications \
    --object-path /org/freedesktop/Notifications \
    --interface org.freedesktop.Notifications \
    --method Notify \
    "MyApp" 0 "dialog-information" \
    "Hello" "This is a notification" \
    "" "" "" -1
```

### 示例 4: 复杂数据结构

#### 使用 busctl

```bash
busctl --user call com.example.Service /com/example/Object \
    com.example.Interface Method \
    "siai(is)a{sv}" \
    "test" 42 3 1 2 3 \
    100 200 "nested" \
    2 "name" "s" "John" "age" "i" 30
```

#### 使用 gdbus

```bash
gdbus call --session \
    --dest com.example.Service \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --method Method \
    "'test' 42 [1, 2, 3] (100, (200, 'nested')) {'name': <'John'>, 'age': <30>}"
```

---

## 最佳实践

### 1. 使用 busctl 的技巧

```bash
# 使用 JSON 格式输出，更易解析
busctl --user --json=pretty call ...

# 使用 verbose 模式查看详细信息
busctl --user --verbose call ...

# 设置超时时间
busctl --user --timeout=5000 call ...

# 查看方法签名
busctl --user introspect com.example.Service /com/example/Object
```

### 2. 使用 gdbus 的技巧

```bash
# 使用 emit 发送信号
gdbus emit --session \
    --object-path /com/example/Object \
    --interface com.example.Interface \
    --signal TestSignal "Hello"

# 监控信号
gdbus monitor --session \
    --dest com.example.Service

# 查看接口信息
gdbus introspect --session \
    --dest com.example.Service \
    --object-path /com/example/Object
```

### 3. 调试技巧

```bash
# 使用 busctl 监控所有消息
busctl --user monitor

# 使用 gdbus 监控特定服务
gdbus monitor --session --dest com.example.Service

# 查看服务状态
busctl --user status com.example.Service
```

### 4. 脚本编写

#### busctl 脚本示例

```bash
#!/bin/bash
# 使用 busctl 的脚本

SERVICE="com.example.Service"
OBJECT="/com/example/Object"
INTERFACE="com.example.Interface"

# 调用方法并解析结果
RESULT=$(busctl --user call "$SERVICE" "$OBJECT" "$INTERFACE" Method "s" "test")
echo "Result: $RESULT"

# 使用 JSON 格式解析
RESULT=$(busctl --user --json=short call "$SERVICE" "$OBJECT" "$INTERFACE" Method "s" "test")
echo "JSON Result: $RESULT"
```

#### gdbus 脚本示例

```bash
#!/bin/bash
# 使用 gdbus 的脚本

SERVICE="com.example.Service"
OBJECT="/com/example/Object"
INTERFACE="com.example.Interface"

# 调用方法
gdbus call --session \
    --dest "$SERVICE" \
    --object-path "$OBJECT" \
    --interface "$INTERFACE" \
    --method Method \
    "test"
```

---

## 总结

### busctl 优势
- ✅ 更简洁的语法
- ✅ 原生 JSON 支持
- ✅ 更好的脚本集成
- ✅ 更精确的类型控制
- ✅ 更好的性能

### gdbus 优势
- ✅ 更易读的语法
- ✅ 更好的交互体验
- ✅ 更直观的数据表示
- ✅ 更丰富的功能
- ✅ 更好的文档

### 选择建议
- **自动化脚本**：使用 busctl
- **交互式调试**：使用 gdbus
- **系统管理**：使用 busctl
- **开发测试**：使用 gdbus

两个工具都支持所有 D-Bus 数据类型，选择哪个主要取决于个人偏好和使用场景。