# KenC# 语言规范

## 概述

KenC# 是专为 KenuxOS 设计的编程语言，语法类似 C 语言，编译后生成 `.ken` 文件，源文件扩展名为 `.kenp`。

## 文件扩展名

- 源文件：`.kenp` (KenC# Precompiled)
- 编译后文件：`.ken` (KenC# Executable)

## 基本语法

### 注释

```kenp
// 单行注释

/*
多行注释
*/
```

### 数据类型

#### 基本类型

```kenp
int        // 32位整数
long       // 64位整数
float      // 32位浮点数
double     // 64位浮点数
char       // 字符
string     // 字符串
bool       // 布尔值 (true/false)
void       // 无类型
```

#### 复合类型

```kenp
array<T>   // 数组
struct     // 结构体
enum       // 枚举
```

### 变量声明

```kenp
int age = 25;
string name = "KenuxOS";
float price = 99.99;
bool isActive = true;

// 类型推断
var count = 100;
var message = "Hello";

// 常量
const int MAX_SIZE = 1024;
const string VERSION = "1.0.0";
```

### 数组

```kenp
// 声明数组
int numbers[10];
string names[5];

// 初始化数组
int primes[] = {2, 3, 5, 7, 11};
string colors[] = {"red", "green", "blue"};

// 动态数组
array<int> dynamicArray;
dynamicArray.push(10);
dynamicArray.push(20);

// 访问数组元素
int first = numbers[0];
numbers[1] = 42;

// 数组长度
int len = numbers.length;
```

### 结构体

```kenp
struct Point {
    int x;
    int y;
}

struct Person {
    string name;
    int age;
    float height;
}

// 使用结构体
Point p;
p.x = 10;
p.y = 20;

Person person;
person.name = "Alice";
person.age = 30;
person.height = 1.75;
```

### 枚举

```kenp
enum Color {
    Red,
    Green,
    Blue
}

enum Day {
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
}

// 使用枚举
Color favoriteColor = Color.Blue;
Day today = Day.Wednesday;
```

## 运算符

### 算术运算符

```kenp
+   // 加法
-   // 减法
*   // 乘法
/   // 除法
%   // 取模
++  // 自增
--  // 自减
```

### 比较运算符

```kenp
==  // 等于
!=  // 不等于
<   // 小于
>   // 大于
<=  // 小于等于
>=  // 大于等于
```

### 逻辑运算符

```kenp
&&  // 逻辑与
||  // 逻辑或
!   // 逻辑非
```

### 位运算符

```kenp
&   // 按位与
|   // 按位或
^   // 按位异或
~   // 按位取反
<<  // 左移
>>  // 右移
```

### 赋值运算符

```kenp
=   // 赋值
+=  // 加赋值
-=  // 减赋值
*=  // 乘赋值
/=  // 除赋值
%=  // 取模赋值
&=  // 按位与赋值
|=  // 按位或赋值
^=  // 按位异或赋值
<<= // 左移赋值
>>= // 右移赋值
```

## 控制流

### 条件语句

```kenp
// if 语句
if (condition) {
    // 代码块
}

// if-else 语句
if (condition) {
    // 条件为真时执行
} else {
    // 条件为假时执行
}

// if-else if-else 语句
if (condition1) {
    // 条件1为真时执行
} else if (condition2) {
    // 条件2为真时执行
} else {
    // 所有条件为假时执行
}

// 三元运算符
int max = (a > b) ? a : b;
```

### 循环语句

```kenp
// for 循环
for (int i = 0; i < 10; i++) {
    // 循环体
}

// while 循环
while (condition) {
    // 循环体
}

// do-while 循环
do {
    // 循环体
} while (condition);

// for-each 循环
array<int> numbers = {1, 2, 3, 4, 5};
for (int num : numbers) {
    // num 为数组中的每个元素
}

// break 和 continue
for (int i = 0; i < 10; i++) {
    if (i == 5) {
        break;  // 跳出循环
    }
    if (i % 2 == 0) {
        continue;  // 跳过本次迭代
    }
}
```

### switch 语句

```kenp
switch (expression) {
    case value1:
        // 代码块
        break;
    case value2:
        // 代码块
        break;
    default:
        // 默认代码块
        break;
}
```

## 函数

### 函数定义

```kenp
// 基本函数定义
int add(int a, int b) {
    return a + b;
}

// 无返回值函数
void greet(string name) {
    print("Hello, " + name + "!");
}

// 带默认参数
int multiply(int a, int b = 2) {
    return a * b;
}

// 可变参数函数
int sum(int count, ...) {
    int total = 0;
    // 处理可变参数
    return total;
}

// 函数指针
typedef int (*Operation)(int, int);
int compute(int a, int b, Operation op) {
    return op(a, b);
}
```

### 函数调用

```kenp
int result = add(5, 3);
greet("KenuxOS");
int product = multiply(4);
```

### 递归函数

```kenp
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}
```

## 类和对象

### 类定义

```kenp
class Animal {
    // 私有成员
    private string name;
    private int age;

    // 公共成员
    public:
        // 构造函数
        Animal(string name, int age) {
            this.name = name;
            this.age = age;
        }

        // 析构函数
        ~Animal() {
            // 清理代码
        }

        // 方法
        void speak() {
            print("Animal sound");
        }

        string getName() {
            return this.name;
        }

        void setName(string name) {
            this.name = name;
        }
}
```

### 继承

```kenp
class Dog extends Animal {
    private string breed;

    public:
        Dog(string name, int age, string breed) : Animal(name, age) {
            this.breed = breed;
        }

        void speak() {
            print("Woof!");
        }

        void fetch() {
            print("Fetching...");
        }
}
```

### 接口

```kenp
interface Drawable {
    void draw();
}

interface Movable {
    void move(int x, int y);
}

class Shape implements Drawable, Movable {
    public:
        void draw() {
            // 实现绘制逻辑
        }

        void move(int x, int y) {
            // 实现移动逻辑
        }
}
```

### 抽象类

```kenp
abstract class Shape {
    abstract void draw();
    abstract double area();

    void setColor(string color) {
        // 通用方法
    }
}

class Circle extends Shape {
    private double radius;

    public:
        void draw() {
            // 实现绘制圆形
        }

        double area() {
            return 3.14159 * radius * radius;
        }
}
```

## 异常处理

```kenp
try {
    // 可能抛出异常的代码
    int result = divide(a, b);
} catch (DivideByZeroException e) {
    print("Division by zero!");
} catch (Exception e) {
    print("An error occurred: " + e.message);
} finally {
    // 总是执行的代码
    print("Cleanup");
}

// 抛出异常
throw new Exception("Something went wrong");
```

## 泛型

```kenp
// 泛型函数
T max<T>(T a, T b) {
    return (a > b) ? a : b;
}

// 泛型类
class Container<T> {
    private T value;

    public:
        Container(T value) {
            this.value = value;
        }

        T getValue() {
            return this.value;
        }

        void setValue(T value) {
            this.value = value;
        }
}

// 使用泛型
Container<int> intContainer = new Container<int>(42);
Container<string> stringContainer = new Container<string>("Hello");
```

## 命名空间

```kenp
namespace Graphics {
    class Point {
        // ...
    }

    void drawPoint(Point p) {
        // ...
    }
}

// 使用命名空间
using Graphics;

Point p = new Point(10, 20);
drawPoint(p);

// 完全限定名
Graphics.Point p = new Graphics.Point(10, 20);
```

## 模块和导入

```kenp
// 导入模块
import std.io;
import std.math;
import kenux.graphics;
import kenux.network;

// 导入特定成员
import std.io.print;
import std.math.sqrt;

// 模块导出
module mymodule {
    export func1;
    export func2;
}
```

## KenuxOS API 绑定

### 图形 API

```kenp
import kenux.graphics;

void main() {
    graphics_extended_init();
    
    // 绘制图形
    rgba_color_t color = {255, 0, 0, 255};
    graphics_draw_circle(100, 100, 50, color, 1);
    
    // 绘制位图
    bitmap_t* bitmap = load_bitmap("image.png");
    graphics_draw_bitmap(bitmap, 0, 0);
}
```

### 多媒体 API

```kenp
import kenux.multimedia;

void main() {
    // 播放音频
    audio_buffer_t* audio = audio_load("music.mp3");
    audio_play(audio);
    
    // 捕获摄像头
    camera_init(0);
    camera_start(640, 480, 30);
    image_buffer_t* frame = camera_capture_frame();
}
```

### 网络 API

```kenp
import kenux.network;

void main() {
    // HTTP 请求
    http_client_t* client = http_client_create("https://api.example.com");
    http_response_t* response = http_get(client, "/data");
    
    // WebSocket 连接
    websocket_t* ws = websocket_connect("wss://echo.websocket.org");
    websocket_send_text(ws, "Hello, WebSocket!");
}
```

### 数据库 API

```kenp
import kenux.database;

void main() {
    // 打开数据库
    database_t* db = database_open("data.db", DB_TYPE_SQLITE);
    
    // 执行查询
    db_result_t* result = database_query(db, "SELECT * FROM users");
    
    // 键值存储
    kv_store_t* store = kv_open("cache.kv");
    kv_set_string(store, "key", "value");
}
```

### 加密 API

```kenp
import kenux.crypto;

void main() {
    // 哈希
    hash256_t hash;
    crypto_hash_sha256(data, len, &hash);
    
    // 加密
    aes256_key_t key;
    iv_t iv;
    crypto_aes256_encrypt(plaintext, len, &key, &iv, ciphertext, CRYPTO_AES_GCM);
}
```

### 线程 API

```kenp
import kenux.thread;

void worker(void* arg) {
    print("Worker thread running");
}

void main() {
    // 创建线程
    thread_t* t = thread_create(worker, null, "worker");
    thread_join(t);
    
    // 线程池
    thread_pool_t* pool = thread_pool_create(4);
    thread_pool_submit(pool, task);
}
```

### 设备 API

```kenp
import kenux.device;

void main() {
    // 传感器
    sensor_t* accel = sensor_open(SENSOR_ACCELEROMETER, 0);
    accelerometer_data_t data;
    sensor_read_accelerometer(accel, &data);
    
    // 蓝牙
    bluetooth_device_t** devices;
    int count;
    bluetooth_scan(&count);
}
```

### 机器学习 API

```kenp
import kenux.ml;

void main() {
    // 创建神经网络
    neural_network_t* nn = neural_network_create(OPTIMIZER_ADAM, 0.001);
    
    // 添加层
    dense_layer_t* layer1 = dense_layer_create(784, 128, ACTIVATION_RELU);
    neural_network_add_layer(nn, layer1);
    
    // 训练
    neural_network_train(nn, dataset, 100, 32);
}
```

## 预处理器指令

```kenp
#define PI 3.14159
#define MAX_SIZE 1024

#ifdef DEBUG
    print("Debug mode");
#endif

#if OS == "KenuxOS"
    // KenuxOS 特定代码
#endif

#include "header.kenp"
```

## 属性和注解

```kenp
@deprecated
void oldFunction() {
    // 此函数已弃用
}

@entry
void main() {
    // 程序入口点
}

@async
void asyncFunction() {
    // 异步函数
}

@inline
int fastAdd(int a, int b) {
    return a + b;
}
```

## 内存管理

```kenp
// 手动内存管理
int* ptr = malloc(sizeof(int) * 10);
free(ptr);

// 智能指针
SmartPtr<int> smartPtr = new SmartPtr<int>(new int(42));

// 引用计数
RefPtr<string> ref = new RefPtr<string>("Hello");
```

## 并发编程

```kenp
// 协程
async void fetchData() {
    string data = await http_get(url);
    print(data);
}

// 锁
mutex_t* mutex = mutex_create();
mutex_lock(mutex);
// 临界区
mutex_unlock(mutex);

// 通道
channel_t* chan = channel_create(10, sizeof(int));
channel_send(chan, &value);
channel_receive(chan, &result);
```

## 标准库

### 字符串操作

```kenp
string str = "Hello, KenuxOS!";
int len = str.length();
string sub = str.substring(0, 5);
string[] parts = str.split(",");
string upper = str.toUpper();
string lower = str.toLower();
bool contains = str.contains("KenuxOS");
```

### 数学函数

```kenp
import std.math;

double result = sqrt(16.0);
double angle = sin(PI / 2);
double power = pow(2, 10);
int absValue = abs(-42);
```

### 文件 I/O

```kenp
import std.io;

File* file = File.open("data.txt", "r");
string content = file.readAll();
file.close();

File* outFile = File.open("output.txt", "w");
outFile.write("Hello, World!");
outFile.close();
```

### 时间和日期

```kenp
import std.time;

Time now = Time.now();
int timestamp = now.toTimestamp();
string formatted = now.format("YYYY-MM-DD HH:mm:ss");
```

## 编译指令

```kenp
// 编译为 .ken 文件
#compile output.ken

// 优化级别
#optimize 2

// 目标平台
#target kenuxos-x86_64

// 调试信息
#debug

// 链接库
#link kenux.graphics
#link kenux.network
```

## 示例程序

### Hello World

```kenp
import std.io;

void main() {
    print("Hello, KenuxOS!");
}
```

### 简单计算器

```kenp
import std.io;
import std.math;

float add(float a, float b) {
    return a + b;
}

float subtract(float a, float b) {
    return a - b;
}

float multiply(float a, float b) {
    return a * b;
}

float divide(float a, float b) {
    if (b == 0) {
        throw new Exception("Division by zero");
    }
    return a / b;
}

void main() {
    float x = 10.5;
    float y = 5.0;
    
    print("x + y = " + add(x, y));
    print("x - y = " + subtract(x, y));
    print("x * y = " + multiply(x, y));
    print("x / y = " + divide(x, y));
}
```

### 图形界面程序

```kenp
import kenux.graphics;
import kenux.desktop;

void main() {
    desktop_init();
    
    desktop_window_t* window = desktop_create_window(100, 100, 400, 300, "My App", 0);
    
    rgba_color_t color = {100, 150, 200, 255};
    graphics_draw_circle(200, 150, 50, color, 1);
    
    desktop_run();
}
```

### 网络客户端

```kenp
import kenux.network;
import std.io;

void main() {
    http_client_t* client = http_client_create("https://api.example.com");
    
    http_response_t* response = http_get(client, "/users");
    
    if (response->status_code == 200) {
        string body = http_response_get_body_string(response);
        print(body);
    }
    
    http_response_free(response);
    http_client_free(client);
}
```

## 语言特性总结

- **静态类型**：编译时类型检查
- **面向对象**：支持类、继承、接口
- **泛型**：类型安全的泛型编程
- **异常处理**：结构化的错误处理
- **模块化**：命名空间和模块系统
- **并发**：线程、协程、通道
- **内存安全**：智能指针和垃圾回收
- **KenuxOS 集成**：直接访问系统 API
- **跨平台**：编译为 .ken 格式，可在 KenuxOS 上运行