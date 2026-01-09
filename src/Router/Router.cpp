#include "Router.h"
#include <sstream>
#include <iostream>
#include <algorithm>

// HTTP请求解析实现
bool HttpRequest::parse(const std::string& rawRequest) {
    std::istringstream iss(rawRequest);
    std::string firstLine;
    
    // 读取请求首行
    if (!std::getline(iss, firstLine)) {
        return false;
    }
    
    // 解析请求首行：GET /index.html?name=test HTTP/1.1
    std::istringstream firstLineIss(firstLine);
    if (!(firstLineIss >> method >> fullPath >> version)) {
        return false;
    }
    
    // 分离路径和查询参数
    size_t queryPos = fullPath.find('?');
    if (queryPos != std::string::npos) {
        path = fullPath.substr(0, queryPos);
        std::string queryString = fullPath.substr(queryPos + 1);
        params = Router::parseParams(queryString); //parameters
    } else {
        path = fullPath;
        params.clear();
    }
    
    // 后续可以添加请求头和请求体的解析 //原来我没有这些
    
    return true;
}

// 获取指定名称的参数值
std::string HttpRequest::getParam(const std::string& name) const {
    for (const auto& param : params) {
        if (param.first == name) {
            return param.second;
        }
    }
    return "";
}

// HTTP响应构造
std::string HttpResponse::toString() const {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << "\r\n";
    oss << "Content-Type: " << contentType << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    oss << body;
    return oss.str();
}

// 设置200 OK响应
void HttpResponse::setOK(const std::string& contentType, const std::string& body) {
    this->status = "200 OK";
    this->contentType = contentType;
    this->body = body;
}

// 设置404 Not Found响应
void HttpResponse::setNotFound() {
    this->status = "404 Not Found";
    this->contentType = "text/html; charset=utf-8";
    this->body = "<html><h1>404 Not Found</h1><p>文件不存在！</p></html>";
}

// 设置403 Forbidden响应
void HttpResponse::setForbidden() {
    this->status = "403 Forbidden";
    this->contentType = "text/html; charset=utf-8";
    this->body = "<html><h1>403 Forbidden</h1><p>路径不安全！</p></html>";
}

// Router类实现
Router::Router() {
    // 初始化路由表
}

// 获取单例实例
Router& Router::getInstance() {
    static Router instance;
    return instance;
}

// 注册GET路由
void Router::get(const std::string& path, RequestHandler handler) {
    routes["GET"].push_back({path, handler});           //路由表 struct Route = {path, handler}
    std::cout << "注册路由：GET " << path << std::endl;
}

// 注册POST路由
// void Router::post(const std::string& path, RequestHandler handler) {
//     routes["POST"].push_back({path, handler});
//     std::cout << "注册路由：POST " << path << std::endl;
// }

// 处理请求
void Router::handleRequest(const HttpRequest& request, HttpResponse& response) {
    // 检查是否支持该请求方法
    auto methodIt = routes.find(request.method); // ？
    if (methodIt == routes.end()) {                 //？
        response.status = "405 Method Not Allowed";
        response.contentType = "text/html; charset=utf-8";
        response.body = "<html><h1>405 Method Not Allowed</h1></html>";
        return;
    }
    
    // 遍历该方法下的所有路由，寻找匹配的路径
    for (const auto& route : methodIt->second) {
        if (matchPath(route.path, request.path)) {
            // 找到匹配的路由，调用处理函数
            route.handler(request, response);  // ？ handler怎么知道匹配哪个
            return;
        }
    }
    
    // 未找到匹配的路由
    response.setNotFound();
}

// 解析URL参数
UrlParams Router::parseParams(const std::string& queryString) {
    UrlParams params;
    std::istringstream iss(queryString);
    std::string param;
    
    while (std::getline(iss, param, '&')) {
        size_t equalsPos = param.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = param.substr(0, equalsPos); //1
            std::string value = param.substr(equalsPos + 1);
            params.emplace_back(key, value); //1 //emplace_back ？
        }
    }
    
    return params;
}

// 检查路径是否匹配
bool Router::matchPath(const std::string& routePath, const std::string& requestPath) {
    // 简单实现：完全匹配
    // 后续可以扩展为支持通配符、正则表达式等
    return routePath == requestPath;
}