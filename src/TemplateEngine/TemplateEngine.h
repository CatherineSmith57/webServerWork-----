#ifndef TEMPLATE_ENGINE_H
#define TEMPLATE_ENGINE_H

#include <string>
#include <map>
#include <regex>

// 模板引擎类，用于处理HTML模板中的{{ }}语法替换
class TemplateEngine {
public:
    // 构造函数
    TemplateEngine();
    
    // 析构函数
    ~TemplateEngine();
    
    // 设置模板内容
    void setTemplate(const std::string& templateContent);
    
    // 设置变量值
    void setVariable(const std::string& key, const std::string& value);
    
    // 渲染模板，返回替换后的内容
    std::string render();
    
    // 从文件加载模板
    bool loadTemplateFromFile(const std::string& filePath);
    
private:
    std::string templateContent;           // 模板内容
    std::map<std::string, std::string> variables; // 变量映射表
    std::regex variableRegex;              // 正则表达式，用于匹配{{ variable }}语法 
};

#endif // TEMPLATE_ENGINE_H