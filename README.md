# C++ WebServer 期末作业

这是一个基于C++和Winsock库开发的简易Web服务器，用于处理HTTP请求并提供静态文件服务和动态内容生成。该项目是计算机类专业学生的期末作业，实现了基本的Web服务器功能、路由机制和安全防护。

## 功能特性

- ✅ 支持HTTP/1.1协议的GET请求
- ✅ 静态文件服务（HTML、CSS、JavaScript、图片、视频、音频等）
- ✅ 动态内容生成（基于Controller的路由机制）
- ✅ 自动识别文件MIME类型
- ✅ 路径遍历攻击防护
- ✅ URL参数解析支持
- ✅ 友好的错误页面（404、403、405）
- ✅ 模块化设计，易于扩展

## 技术栈

- **编程语言**：C++
- **网络库**：Winsock 2.2
- **编译工具**：GCC (MinGW) 或 MSVC
- **目标平台**：Windows
- **设计模式**：MVC架构、单例模式

## 项目结构

```
webServerWork/
├── mySockets.cpp          # Web服务器主程序
├── mySockets.exe          # 编译后的可执行文件
├── Controller.h/cpp       # 控制器类，处理业务逻辑
├── Router.h/cpp           # 路由系统，处理URL映射
├── README.md              # 项目说明文档
├── aboutcss.html          # 额外的HTML文件
├── teacher/               # 老师提供的参考代码
│   ├── router.framework.cpp/h   # 路由框架参考
│   ├── router.mysite.cpp        # 路由实现参考
│   └── socket.windows.client/server.cpp  # Socket编程参考
└── www/                   # 网页资源根目录
    ├── index.html         # 主网页
    ├── style.css          # 样式文件
    ├── 1031.html          # 其他网页
    ├── 1031dec.css        # 其他样式
    ├── lastDayOfOct.html  # 其他网页
    ├── lastdoo.css        # 其他样式
    ├── learn.html         # 学习笔记网页
    ├── learn.css          # 学习笔记样式
    ├── image/             # 图片资源
    │   ├── champion.jpg
    │   ├── football.jpg
    │   ├── pudong.jpg
    │   └── ...
    ├── video/             # 视频资源
    │   ├── 7-1.mp4
    │   └── pianoNoveky.mp4
    └── music/             # 音频资源
        ├── 道别 😰.wav
        └── 道别是件难事.wav
```

## 编译和运行

### 编译步骤

1. 确保已安装GCC (MinGW) 或 MSVC编译环境
2. 打开命令行终端，进入项目目录
3. 执行以下编译命令：

```bash
# 使用GCC编译
g++ -o mySockets.exe mySockets.cpp Controller.cpp Router.cpp -lws2_32

# 使用MSVC编译（Visual Studio命令提示符）
cl mySockets.cpp Controller.cpp Router.cpp ws2_32.lib
```

### 运行服务器

1. 编译成功后，执行生成的可执行文件：

```bash
./mySockets.exe
```

2. 服务器将在端口8080上启动，输出类似以下信息：

```
注册路由：GET /
注册路由：GET /welcome
注册路由：GET /students
注册路由：GET /students/detail
注册路由：GET /courses
注册路由：GET /courses/detail
WebServer启动成功！访问 http://localhost:8080
动态路由已注册：
  - GET /                  首页
  - GET /welcome           欢迎页
  - GET /students          学生列表
  - GET /students/detail   学生详情
  - GET /courses           课程列表
  - GET /courses/detail    课程详情
静态文件服务：支持HTML、CSS、JS、图片、视频、音频等
```

3. 在浏览器中访问：

```
http://localhost:8080          # 访问动态首页
http://localhost:8080/index.html  # 访问静态首页
http://localhost:8080/students     # 访问学生列表
```

## 核心功能说明

### 1. 动态路由系统

服务器实现了基于Controller的路由机制，将不同URL路径映射到对应的处理函数：

| 路径 | 方法 | 功能 |
|------|------|------|
| `/` | GET | 动态生成的首页 |
| `/welcome` | GET | 动态生成的欢迎页 |
| `/students` | GET | 学生列表 |
| `/students/detail` | GET | 学生详情 |
| `/courses` | GET | 课程列表 |
| `/courses/detail` | GET | 课程详情 |

### 2. 静态文件服务

支持多种文件类型的静态服务，自动识别MIME类型：

- **HTML/CSS/JS**：网页和脚本文件
- **图片**：JPG、PNG、WEBP等
- **视频**：MP4等
- **音频**：WAV等

### 3. URL参数解析

支持URL查询参数，如：

```
http://localhost:8080/?name=张三&age=20
http://localhost:8080/students/detail?stuid=1001
```

### 4. 安全防护

- **路径遍历防护**：防止恶意路径访问服务器文件系统
- **输入验证**：对HTTP请求进行基本格式验证
- **资源限制**：请求缓冲区大小限制为4096字节

## 代码架构

### 1. 核心类设计

- **HttpRequest**：封装HTTP请求，包含方法、路径、参数等
- **HttpResponse**：封装HTTP响应，包含状态码、内容类型、响应体等
- **Router**：路由系统，处理URL映射和请求分发
- **Controller**：控制器基类，派生出具体的业务控制器
  - **MainController**：处理首页和欢迎页
  - **StudentController**：处理学生相关请求
  - **CourseController**：处理课程相关请求

### 2. 请求处理流程

1. **客户端请求**：浏览器发送HTTP请求到服务器
2. **Socket接收**：服务器通过Socket接收请求
3. **请求解析**：解析HTTP请求，提取方法、路径、参数等
4. **路由匹配**：根据路径匹配对应的处理函数
5. **业务处理**：调用Controller处理业务逻辑
6. **响应生成**：生成HTTP响应
7. **响应发送**：通过Socket发送响应给客户端
8. **连接关闭**：关闭客户端连接

## 老师参考代码说明

老师提供的参考代码位于`teacher/`目录下，包含以下内容：

- **socket.windows.server.cpp**：Socket服务器编程参考，展示了基本的Socket初始化、绑定、监听和连接处理
- **socket.windows.client.cpp**：Socket客户端编程参考
- **router.framework.cpp/h**：路由框架设计参考
- **router.mysite.cpp**：路由实现参考

这些代码可以帮助你理解Socket编程和路由设计的基本原理。

## 使用示例

### 访问动态首页

```bash
http://localhost:8080/
```

### 访问带参数的首页

```bash
http://localhost:8080/?name=张三
```

### 访问学生列表

```bash
http://localhost:8080/students
```

### 访问特定学生详情

```bash
http://localhost:8080/students/detail?stuid=1001
```

### 访问静态HTML文件

```bash
http://localhost:8080/index.html
http://localhost:8080/1031.html
```

### 访问图片资源

```bash
http://localhost:8080/image/pudong.jpg
```

### 访问视频资源

```bash
http://localhost:8080/video/pianoNoveky.mp4
```

## 代码说明

### 1. 主程序入口 (mySockets.cpp)

- **InitWinsock()**：初始化Winsock环境
- **main()**：程序主入口，包含Socket创建、绑定、监听和连接处理
- **HandleClient()**：处理单个客户端请求
- **initControllers()**：初始化控制器和路由

### 2. 路由系统 (Router.cpp/h)

- **Router::get()/post()**：注册路由
- **Router::handleRequest()**：处理请求，分发到对应的处理函数
- **Router::parseParams()**：解析URL参数
- **Router::matchPath()**：匹配路径

### 3. 控制器 (Controller.cpp/h)

- **MainController**：处理首页和欢迎页
- **StudentController**：处理学生相关请求
- **CourseController**：处理课程相关请求

## 扩展建议

1. **添加POST请求支持**：处理表单提交和数据上传
2. **实现会话管理**：支持用户登录和状态保持
3. **添加数据库支持**：使用SQLite或MySQL存储数据
4. **实现模板引擎**：使用模板文件生成动态内容
5. **添加日志系统**：记录访问日志和错误日志
6. **实现多线程**：提高服务器并发处理能力
7. **添加HTTPS支持**：实现SSL/TLS加密
8. **支持WebSocket**：实现实时通信

## 常见问题

### Q: 编译时出现"undefined reference to `WSAStartup@8'"错误？
A: 这是因为没有链接Winsock库。请确保在编译命令中添加`-lws2_32`参数。

### Q: 运行时出现"初始化Winsock失败"错误？
A: 请检查Windows系统的网络服务是否正常，或者尝试以管理员身份运行程序。

### Q: 浏览器无法访问服务器？
A: 请检查：
1. 服务器是否正在运行
2. 防火墙是否允许8080端口访问
3. 浏览器地址是否正确（http://localhost:8080）

### Q: 静态文件无法访问？
A: 请检查：
1. 文件是否存在于www目录下
2. 文件路径是否正确
3. 文件名大小写是否正确（Windows下不区分大小写，但建议保持一致）

## 编译和运行命令

### 编译命令

```bash
g++ -o mySockets.exe mySockets.cpp Controller.cpp Router.cpp -lws2_32
```

### 运行命令

```bash
./mySockets.exe
```

### 测试命令

```bash
# 测试首页
curl http://localhost:8080/

# 测试带参数的首页
curl "http://localhost:8080/?name=张三"

# 测试静态文件
curl http://localhost:8080/index.html
```

## 期末作业要求

### 1. 基本要求
- [x] 实现HTTP服务器，支持静态文件服务
- [x] 实现路由机制，支持动态内容生成
- [x] 实现URL参数解析
- [x] 实现安全防护机制
- [x] 代码结构清晰，易于维护

### 2. 扩展要求
- [ ] 实现多线程支持
- [ ] 实现POST请求支持
- [ ] 实现数据库连接
- [ ] 实现日志系统

## 作者信息

- **作者**：Zoe Tian
- **专业**：计算机科学与技术
- **年级**：大一
- **日期**：2025年12月
- **学校**：武汉大学

## 许可证

本项目仅供学习和教育使用，可自由修改和扩展。

---

**期末作业项目** | **C++ Web Server** | **2025**