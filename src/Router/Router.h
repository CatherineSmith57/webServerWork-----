#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <utility>

// 前向声明
class HttpRequest;
class HttpResponse;

// 处理函数类型定义
typedef std::function<void(const HttpRequest&, HttpResponse&)> RequestHandler; // 处理函数类型，参数是HttpRequest和HttpResponse引用
//这是什么 ？ 这是一个函数指针，指向一个函数，这个函数的参数是HttpRequest和HttpResponse引用，返回值是void

// URL参数类型，键值对
using UrlParams = std::vector<std::pair<std::string, std::string>>; // URL参数类型，键值对，例如name=test&age=18

// HTTP请求类
class HttpRequest {
public:
    std::string method;      // 请求方法：GET, POST等
    std::string path;        // 请求路径：/index.html
    std::string fullPath;    // 完整路径，包含查询参数：/index.html?name=test
    std::string version;     // HTTP版本：HTTP/1.1
    UrlParams params;        // URL参数：name=test&age=18
    
    // 从原始请求字符串解析HTTP请求
    bool parse(const std::string& rawRequest);
    
    // 获取指定名称的参数值
    std::string getParam(const std::string& name) const;
};

// HTTP响应类
class HttpResponse {
public:
    std::string status;      // 状态码：200 OK, 404 Not Found等
    std::string contentType; // 内容类型：text/html, application/json等
    std::string body;        // 响应体内容
    
    // 构造HTTP响应字符串
    std::string toString() const;
    
    // 设置200 OK响应
    void setOK(const std::string& contentType, const std::string& body);
    
    // 设置404 Not Found响应
    void setNotFound();
    
    // 设置403 Forbidden响应
    void setForbidden();
};

// 路由类
class Router {
public:
    // 单例模式，获取唯一实例
    static Router& getInstance();
    
    // 禁止拷贝和赋值
    Router(const Router&) = delete;     // 单例
    Router& operator=(const Router&) = delete;  // 单例
    
    // 注册路由，支持GET方法
    void get(const std::string& path, RequestHandler handler);
    
    // 注册路由，支持POST方法
    //void post(const std::string& path, RequestHandler handler);
    
    // 处理请求，根据路径找到对应的处理函数
    void handleRequest(const HttpRequest& request, HttpResponse& response);
    
    // 解析URL参数
    static UrlParams parseParams(const std::string& queryString);
    
private:
    Router(); // 私有构造函数
    
    // 路由表项，包含路径和处理函数
    struct Route {
        std::string path;
        RequestHandler handler;
    };
    
    // 路由映射，按方法分类
    std::map<std::string, std::vector<Route>> routes;
    
    // 检查路径是否匹配
    bool matchPath(const std::string& routePath, const std::string& requestPath);
};

#endif // ROUTER_H