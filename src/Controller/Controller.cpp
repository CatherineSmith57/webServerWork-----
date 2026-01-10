#include "Controller.h"
#include "../TemplateEngine/TemplateEngine.h"
#include <functional>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <Windows.h>

// GBK到UTF-8的转换函数
std::string gbkToUtf8(const std::string& gbkStr) {
    std::string utf8Str;
    int len = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    utf8Str = str;
    delete[] wstr;
    delete[] str;
    return utf8Str;
}

// UTF-8到GBK的转换函数（备用）
std::string utf8ToGbk(const std::string& utf8Str) {
    std::string gbkStr;
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, wstr, len);
    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
    gbkStr = str;
    delete[] wstr;
    delete[] str;
    return gbkStr;
}



// 读取课程数据
std::vector<std::vector<std::string>> readCoursesFromFile(const std::string& filePath) {
    std::vector<std::vector<std::string>> courses;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return courses;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> course;
        std::stringstream ss(line);
        std::string field;
        
        while (std::getline(ss, field, ',')) {
            // 去除字段前后的空格
            size_t start = field.find_first_not_of(" \t");
            size_t end = field.find_last_not_of(" \t");
            if (start != std::string::npos && end != std::string::npos) {
                field = field.substr(start, end - start + 1);
            }
            course.push_back(field);
        }
        
        if (!course.empty()) {
            courses.push_back(course);
        }
    }
    
    file.close();
    return courses;
}

// 读取书籍数据
std::vector<std::vector<std::string>> readBooksFromFile(const std::string& filePath) {
    std::vector<std::vector<std::string>> books;
    
    // 使用二进制模式打开文件，确保完整读取
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开文件：" << filePath << std::endl;
        return books;
    }
    
    // 一次性读取整个文件内容
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);
    file.close();
    
    // 将缓冲区转换为字符串
    std::string content(buffer.begin(), buffer.end());
    
    // 检查并移除UTF-8 BOM标记（如果存在）
    if (content.size() >= 3 && 
        content[0] == static_cast<char>(0xEF) && 
        content[1] == static_cast<char>(0xBB) && 
        content[2] == static_cast<char>(0xBF)) {
        content = content.substr(3); // 移除BOM
    }
    
    // 直接处理UTF-8内容（不需要转换，因为文件已经是UTF-8编码）
    std::stringstream ss(content);
    std::string line;
    
    // 逐行读取文件内容
    while (std::getline(ss, line)) {
        // 检查并跳过空行
        if (line.empty()) {
            continue;
        }
        
        std::vector<std::string> book;
        std::string field;
        
        // 自定义分割逻辑，同时支持中文逗号和英文逗号
        size_t start = 0;
        while (start < line.size()) {
            // 查找下一个分隔符位置
            size_t comma_pos = line.find(",", start);
            size_t chinese_comma_pos = line.find("，", start);
            
            // 确定哪个分隔符更靠前
            size_t end;
            if (comma_pos != std::string::npos && chinese_comma_pos != std::string::npos) {
                end = std::min(comma_pos, chinese_comma_pos);
            } else if (comma_pos != std::string::npos) {
                end = comma_pos;
            } else if (chinese_comma_pos != std::string::npos) {
                end = chinese_comma_pos;
            } else {
                // 没有更多分隔符，取剩余部分
                end = line.size();
            }
            
            // 提取字段
            field = line.substr(start, end - start);
            
            // 去除前后空白字符
            size_t trim_start = field.find_first_not_of(" \t");
            size_t trim_end = field.find_last_not_of(" \t");
            if (trim_start != std::string::npos && trim_end != std::string::npos) {
                field = field.substr(trim_start, trim_end - trim_start + 1);
            }
            
            // 直接使用，不需要编码转换（文件已经是UTF-8）
            book.push_back(field);
            
            // 更新start位置，跳过分隔符
            if (end < line.size()) {
                // 检查是中文逗号还是英文逗号
                if (end < line.size() - 2 && 
                    line[end] == static_cast<char>(0xEF) && 
                    line[end+1] == static_cast<char>(0xBC) && 
                    line[end+2] == static_cast<char>(0x8C)) {
                    // 中文逗号（UTF-8是3字节：0xEFBC8C）
                    start = end + 3;
                } else {
                    // 英文逗号（1字节）
                    start = end + 1;
                }
            } else {
                break;
            }
        }
        
        // 确保字段数足够
        while (book.size() < 5) {
            book.push_back("");
        }
        
        // 只取前5个字段
        if (book.size() > 5) {
            book.resize(5);
        }
        
        // 添加到书籍列表
        books.push_back(book);
    }
    
    return books;
}

// 从Book0x.txt中读取书籍详细内容
std::string getBookDetail(const std::string& bookName) {
    // 使用二进制模式打开文件
    std::ifstream file("./Data/text_data/Book0x.txt", std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开文件：./Data/text_data/Book0x.txt" << std::endl;
        return "";
    }
    
    // 一次性读取整个文件内容
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);
    file.close();
    
    // 将缓冲区转换为字符串
    std::string content(buffer.begin(), buffer.end());
    
    // 将GBK编码的整个内容转换为UTF-8
    std::string utf8Content = gbkToUtf8(content);
    
    // 现在处理UTF-8内容
    std::stringstream ss(utf8Content);
    std::string line;
    std::stringstream detailSS;
    bool foundBook = false;
    
    // 逐行读取文件内容
    while (std::getline(ss, line)) {
        // 检查行是否以书名格式开始
        if (line.find("《") != std::string::npos && line.find("》") != std::string::npos) {
            // 如果找到新的书名，停止收集上一个书籍的详情
            if (foundBook) {
                break;
            }
            
            // 检查是否是目标书籍
            if (line.find(bookName) != std::string::npos) {
                foundBook = true;
                // 将书名添加到详情中
                detailSS << line << "\n";
            }
        } else if (foundBook && !line.empty()) {
            // 如果找到目标书籍，收集详细内容
            detailSS << line << "\n";
        }
    }
    
    // 如果找到了内容，返回内容；否则返回空字符串
    std::string result = detailSS.str();
    return result.empty() ? "" : result;
}

// URL解码函数
std::string urlDecode(const std::string& encoded) {
    std::string decoded;
    char ch;
    int i, j;
    for (i = 0; i < encoded.length(); i++) {
        if (encoded[i] == '%') {
            // 处理%xx格式
            sscanf_s(encoded.substr(i + 1, 2).c_str(), "%x", &j);
            ch = static_cast<char>(j);
            decoded += ch;
            i += 2;
        } else if (encoded[i] == '+') {
            // 处理+为空格
            decoded += ' ';
        } else {
            // 其他字符直接添加
            decoded += encoded[i];
        }
    }
    return decoded;
}

// 生成当前时间字符串
std::string getCurrentTime() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    
    return std::string(buffer);
}

// MainController实现
MainController::MainController() {
    registerRoutes();
}

void MainController::registerRoutes() {
    Router& router = Router::getInstance();
    router.get("/", std::bind(&MainController::handleIndex, this, std::placeholders::_1, std::placeholders::_2));
    router.get("/welcome", std::bind(&MainController::handleWelcome, this, std::placeholders::_1, std::placeholders::_2));
    router.get("/static/(.*)", std::bind(&MainController::handleStaticFile, this, std::placeholders::_1, std::placeholders::_2));
}

void MainController::handleIndex(const HttpRequest& request, HttpResponse& response) {
    // 创建模板引擎实例
    TemplateEngine templateEngine;
    
    // 加载模板文件
    if (!templateEngine.loadTemplateFromFile("./Data/Views/index.html")) {
        response.setNotFound();
        return;
    }
    
    // 获取查询参数name并进行URL解码
    std::string nameParam = request.getParam("name");
    nameParam = urlDecode(nameParam);
    
    // 设置模板变量
    templateEngine.setVariable("title", "我的个人网站");
    templateEngine.setVariable("welcome_message", "欢迎访问我的个人网站");
    templateEngine.setVariable("current_time", getCurrentTime());
    templateEngine.setVariable("username", nameParam.empty() ? "访客" : nameParam);
    templateEngine.setVariable("name", "Zoe Tian");
    templateEngine.setVariable("major", "计算机类");
    templateEngine.setVariable("grade", "大一");
    templateEngine.setVariable("student_id", "20250008");
    templateEngine.setVariable("welcome_line", "<p>欢迎你，访客！</p>");


    // 渲染模板
    std::string renderedContent = templateEngine.render();
    
    // 设置响应
    response.setOK("text/html; charset=utf-8", renderedContent);
}

//BookController 实现
void BookController::handleBookDetail(const HttpRequest& request, HttpResponse& response) {
    // 创建模板引擎实例
    TemplateEngine templateEngine;
    
    // 加载模板文件
    if (!templateEngine.loadTemplateFromFile("./Data/Views/book_detail.html")) {
        response.setNotFound();
        return;
    }
    
    // 获取查询参数name并进行URL解码
    std::string bookName = request.getParam("name");
    bookName = urlDecode(bookName);
    
    // 获取书籍详细内容
    std::string bookDetail = getBookDetail(bookName);
    
    // 设置模板变量
    templateEngine.setVariable("title", "书籍详情");
    templateEngine.setVariable("book_name", bookName);
    templateEngine.setVariable("book_detail", bookDetail.empty() ? "暂无详细信息" : bookDetail);
    templateEngine.setVariable("current_time", getCurrentTime());
    
    // 渲染模板
    std::string renderedContent = templateEngine.render();
    
    // 设置响应
    response.setOK("text/html; charset=utf-8", renderedContent);
}

void MainController::handleWelcome(const HttpRequest& request, HttpResponse& response) {
    // 创建模板引擎实例
    TemplateEngine templateEngine;
    
    // 加载模板文件
    if (!templateEngine.loadTemplateFromFile("./Data/Views/index.html")) {
        response.setNotFound();
        return;
    }
    
    // 设置模板变量
    templateEngine.setVariable("title", "欢迎页");
    templateEngine.setVariable("welcome_message", "欢迎使用WebServer");
    templateEngine.setVariable("current_time", getCurrentTime());
    templateEngine.setVariable("username", "访客");
    templateEngine.setVariable("name", "");
    templateEngine.setVariable("major", "");
    templateEngine.setVariable("grade", "");
    templateEngine.setVariable("student_id", "");
    
    // 渲染模板
    std::string renderedContent = templateEngine.render();
    
    // 设置响应
    response.setOK("text/html; charset=utf-8", renderedContent);
}

// 处理静态文件请求
void MainController::handleStaticFile(const HttpRequest& request, HttpResponse& response) {
    // 从路径中提取静态文件路径
    std::string staticPath = request.path.substr(8); // 去掉"/static/"
    std::string filePath = "./Data/Mywww/" + staticPath;
    
    // 读取静态文件
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        response.setNotFound();
        return;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    // 设置响应
    response.setOK("text/html; charset=utf-8", content);
}

// CourseController实现
CourseController::CourseController() {
    registerRoutes();
}

void CourseController::registerRoutes() {
    Router& router = Router::getInstance();
    router.get("/courses", std::bind(&CourseController::handleCourseList, this, std::placeholders::_1, std::placeholders::_2));
    router.get("/courses/detail", std::bind(&CourseController::handleCourseDetail, this, std::placeholders::_1, std::placeholders::_2));
}

void CourseController::handleCourseList(const HttpRequest& request, HttpResponse& response) {
    // 创建模板引擎实例
    TemplateEngine templateEngine;
    
    // 加载模板文件
    if (!templateEngine.loadTemplateFromFile("./Data/Views/courses.html")) {
        response.setNotFound();
        return;
    }
    
    // 获取查询参数name并进行URL解码
    std::string nameParam = request.getParam("name");
    nameParam = urlDecode(nameParam);
    
    // 读取课程数据
    auto courses = readCoursesFromFile("./Data/text_data/courses.txt");
    
    // 生成课程列表HTML
    std::stringstream courseListSS;
    for (const auto& course : courses) {
        if (course.size() >= 2) {
            courseListSS << "<tr><td>" << course[0] << "</td><td>" << course[1] << "</td></tr>\n";
        }
    }
    
    // 设置模板变量
    templateEngine.setVariable("title", "我的课程");
    templateEngine.setVariable("username", nameParam.empty() ? "访客" : nameParam);
    templateEngine.setVariable("current_time", getCurrentTime());
    templateEngine.setVariable("course_list", courseListSS.str());
    
    // 渲染模板
    std::string renderedContent = templateEngine.render();
    
    // 设置响应
    response.setOK("text/html; charset=utf-8", renderedContent);
}

void CourseController::handleCourseDetail(const HttpRequest& request, HttpResponse& response) {
    // 重定向到课程列表
    std::string courseId = request.getParam("courseid");
    response.status = "302 Found";
    response.contentType = "text/html; charset=utf-8";
    response.body = "<html><body><h1>课程详情</h1><p>已重定向到课程列表</p><a href='/courses'>返回课程列表</a></body></html>";
}

// BookController实现
BookController::BookController() {
    registerRoutes();//
}

void BookController::registerRoutes() {
    Router& router = Router::getInstance();
    router.get("/books", std::bind(&BookController::handleBookList, this, std::placeholders::_1, std::placeholders::_2));
    router.get("/books/detail", std::bind(&BookController::handleBookDetail, this, std::placeholders::_1, std::placeholders::_2));
}

void BookController::handleBookList(const HttpRequest& request, HttpResponse& response) {
    // 创建模板引擎实例
    TemplateEngine templateEngine;
    
    // 加载模板文件
    if (!templateEngine.loadTemplateFromFile("./Data/Views/books.html")) {
        response.setNotFound();
        return;
    }
    
    // 获取查询参数name并进行URL解码
    std::string queryName = request.getParam("name");
    queryName = urlDecode(queryName);
    
    // 读取书籍数据
    auto books = readBooksFromFile("./Data/text_data/books.txt");
    
    // 生成书籍列表HTML
    std::stringstream bookListSS;
    for (const auto& book : books) {
        if (book.size() >= 5) {
            // 如果有查询参数，只显示匹配的书籍
            if (!queryName.empty()) {
                if (book[1].find(queryName) == std::string::npos) {
                    continue;
                }
            }
            
            // 获取书籍详细内容
            //std::string bookDetail = getBookDetail(book[1]); //这个好像没用
            
            // 生成书籍列表项，书名作为链接
            bookListSS << "<tr>";
            bookListSS << "<td>" << book[0] << "</td>";
            // 将书名转换为链接，指向书籍详情页面
            bookListSS << "<td><a href='/books/detail?name=" << book[1] << "'>" << book[1] << "</a></td>";
            bookListSS << "<td>" << book[2] << "</td>";
            // 直接显示类型和出版社，不做特殊处理，避免乱码
            bookListSS << "<td>" << book[3] << "</td>";
            bookListSS << "<td>" << book[4] << "</td>";
            bookListSS << "</tr>";
        }
    }
    
    // 设置模板变量
    templateEngine.setVariable("title", "我的书籍");
    
    // 根据是否使用URL参数决定是否显示欢迎信息
    if (queryName.empty()) {
        // 没有使用URL参数，显示欢迎信息
        templateEngine.setVariable("welcome_message", "<p>欢迎你，访客！</p>");
        templateEngine.setVariable("username", "访客");
    } else {
        // 使用了URL参数，不显示欢迎信息
        templateEngine.setVariable("welcome_message", "");
        templateEngine.setVariable("username", queryName);
    }
    
    templateEngine.setVariable("current_time", getCurrentTime());
    templateEngine.setVariable("book_list", bookListSS.str());
    
    // 渲染模板
    std::string renderedContent = templateEngine.render();
    
    // 设置响应
    response.setOK("text/html; charset=utf-8", renderedContent); // 为什么一定是text/html
}