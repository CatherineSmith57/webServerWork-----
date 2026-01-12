#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../Router/Router.h"

// Controller基类
class Controller {
public:
    virtual ~Controller() = default;
    
    // 注册路由，子类需要实现
    virtual void registerRoutes() = 0;
};

// 主页控制器
class MainController : public Controller {
public:
    MainController();
    
    // 注册路由
    void registerRoutes() override;
    
    // 处理首页请求
    void handleIndex(const HttpRequest& request, HttpResponse& response);
    
    // 处理欢迎页请求
    void handleWelcome(const HttpRequest& request, HttpResponse& response);
    
    // 处理静态文件请求
    void handleStaticFile(const HttpRequest& request, HttpResponse& response);
};



// 课程控制器
class CourseController : public Controller {
public:
    CourseController();
    
    // 注册路由
    void registerRoutes() override;
    
    // 处理课程列表请求
    void handleCourseList(const HttpRequest& request, HttpResponse& response);

};

// 书籍控制器
class BookController : public Controller {
public:
    BookController();
    
    // 注册路由
    void registerRoutes() override;
    
    // 处理书籍列表请求
    void handleBookList(const HttpRequest& request, HttpResponse& response);
    
    // 处理书籍详情请求
    void handleBookDetail(const HttpRequest& request, HttpResponse& response);
};

#endif // CONTROLLER_H