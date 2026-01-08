#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>
// Windows网络头文件
#include <winsock2.h>
#include <ws2tcpip.h>
// 链接Winsock库（必须加）
#pragma comment(lib, "ws2_32.lib")

// 引入自定义头文件
#include "../Router/Router.h"
#include "../Controller/Controller.h"

using namespace std;

// 全局配置
const int PORT = 8080;          // 服务器端口
const string WEB_ROOT = "./Data/Mywww"; // 网页文件根目录 ./Data/Mywww

// 初始化Winsock环境
bool InitWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Winsock初始化失败！错误码：" << WSAGetLastError() << endl;
        return false;
    }
    return true;
}

// 获取文件后缀对应的Content-Type（告诉浏览器返回的是什么类型文件）
string GetContentType(const string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == string::npos) return "text/plain"; // 默认纯文本

    string ext = filename.substr(dotPos);
    if (ext == ".html" || ext == ".htm") return "text/html; charset=utf-8";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "application/javascript";
    if (ext == ".png") return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".gif") return "image/gif";
    if (ext == ".mp4") return "video/mp4";
    if (ext == ".mp3") return "audio/mpeg";
    if (ext == ".wav") return "audio/wav";
    if (ext == ".ico") return "image/x-icon";
    return "text/plain";
}

// 读取本地文件内容
bool ReadFile(const string& filepath, string& content) {
    ifstream file(filepath, ios::binary);
    if (!file.is_open()) return false;

    // 读取全部内容
    stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    file.close();
    return true;
}

//防止路径遍历攻击
bool isSafePath(const string& input) {
    // 检查是否包含危险字符，如../或..
    if (input.find("../") != string::npos || input.find("..\\") != string::npos) {
        return false;
    }
    // 检查是否包含绝对路径标记，如C:或D:
    if (input.find(":") != string::npos) {
        return false;
    }
    return true;
}

// 初始化控制器和路由
void initControllers() {
    // 静态初始化，注册路由
    static MainController mainController;
    static CourseController courseController;
    static BookController bookController;
}


// 处理单个客户端请求
void HandleClient(SOCKET clientSock) {
    char reqBuffer[4096] = {0}; // 存储HTTP请求
    // 1. 读取浏览器的HTTP请求
    int recvLen = recv(clientSock, reqBuffer, sizeof(reqBuffer)-1, 0);
    if (recvLen <= 0) {
        closesocket(clientSock);
        return;
    }

    // 2. 解析HTTP请求
    HttpRequest request;
    if (!request.parse(string(reqBuffer, recvLen))) { //parse 解析 改变request对象的成员 请求行 请求头 请求体
        closesocket(clientSock); //关闭连接
        return;
    }

    cout << "收到请求：" << request.method << " " << request.path << endl;
                        //HttpRequest::method       HttpRequest request
                        // 请求方法：GET, POST等     解析HTTP请求
                            
    // 3. 处理请求
    HttpResponse response; // Router
    
    // 先尝试使用路由处理动态请求 //动态优先级更高
    Router::getInstance().handleRequest(request, response); //这里用MVC
    //Router::getInstance() 
    // 如果路由返回404，尝试处理为静态文件请求
    if (response.status == "404 Not Found") {
        string filePath;
        
        // 检查是否是/static/开头的请求 //  /static/是开发者约定的 “静态资源标识前缀”
        if (request.path.find("/static/") == 0) {
            // 静态文件请求，直接映射到Data/Mywww目录
            string staticPath = request.path.substr(8); // 去掉"/static/"  //从字符串的第 8 个索引位置开始，截取后面的内容
            filePath = "./Data/Mywww/" + staticPath;
        } else {
            // 其他路径，直接映射到WEB_ROOT
            filePath = WEB_ROOT + request.path;
        }
        
        // 检查路径是否安全
        if (isSafePath(filePath.substr(WEB_ROOT.length()))) {
            // 尝试读取静态文件
            string content;
            if (ReadFile(filePath, content)) {
                response.setOK(GetContentType(filePath), content);
            } else {
                response.setNotFound();
            }
        } else {
            // 路径不安全，返回403
            response.setForbidden();
        }
    }

    // 4. 发送响应给浏览器
    string respStr = response.toString();
    send(clientSock, respStr.c_str(), respStr.size(), 0);

    // 5. 关闭连接
    closesocket(clientSock);
}

int main() {
    // 1. 初始化Winsock
    if (!InitWinsock()) return 1;

    // 2. 初始化控制器和路由
    initControllers();

    // 3. 创建监听套接字（TCP）
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSock == INVALID_SOCKET) {
        cerr << "创建套接字失败！错误码：" << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // 4. 绑定端口和IP
    SOCKADDR_IN serverAddr = {}; // 初始化结构体
    serverAddr.sin_family = AF_INET;         // IPv4
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY; // 监听所有网卡（正确的访问方式）
    serverAddr.sin_port = htons(PORT);       // 端口转网络字节序
    int ret = ::bind(listenSock, (sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
    if ( ret == SOCKET_ERROR) {
        cerr << "绑定端口失败！错误码：" << WSAGetLastError() << endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    // 5. 开始监听
    if (listen(listenSock, 5) == SOCKET_ERROR) { // 队列长度5
        cerr << "监听失败！错误码：" << WSAGetLastError() << endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    cout << "WebServer启动成功！访问 http://localhost:" << PORT << endl;
    cout << "动态路由已注册：" << endl;
    cout << "  - GET /                  首页" << endl;
    cout << "  - GET /welcome           欢迎页" << endl;
    cout << "  - GET /courses           课程列表" << endl;
    cout << "  - GET /books             书籍列表" << endl;
    cout << "  - GET /static/*          静态文件" << endl;
    cout << "静态文件服务：支持HTML、CSS、JS、图片、视频、音频等" << endl;

    // 6. 循环接收客户端连接
    while (true) {
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSock == INVALID_SOCKET) continue;

        // 处理客户端请求（这里用单线程，作业若要求多线程可改CreateThread）
        HandleClient(clientSock);
    }

    // 清理资源（实际不会执行到，因为while(true)循环）
    closesocket(listenSock);
    WSACleanup();
    return 0;
}