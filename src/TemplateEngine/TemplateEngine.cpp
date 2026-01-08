#include "TemplateEngine.h"
#include <fstream>
#include <sstream>
#include <iostream>

// 构造函数
TemplateEngine::TemplateEngine() : variableRegex(R"(\{\{\s*([a-zA-Z0-9_]+)\s*\}\})", std::regex_constants::ECMAScript) {
    // 初始化正则表达式，匹配{{ variable }}语法
}

// 析构函数
TemplateEngine::~TemplateEngine() {
    // 清理资源
}

// 设置模板内容
void TemplateEngine::setTemplate(const std::string& templateContent) {
    this->templateContent = templateContent;
}

// 设置变量值
void TemplateEngine::setVariable(const std::string& key, const std::string& value) {
    variables[key] = value;
}

// 渲染模板，返回替换后的内容
std::string TemplateEngine::render() {
    std::string result = templateContent;
    std::smatch match;
    
    // 使用正则表达式匹配所有{{ variable }}格式的内容
    std::string tempResult;
    std::string::const_iterator searchStart(result.cbegin());
    std::string::const_iterator startPos = result.cbegin();
    
    while (std::regex_search(searchStart, result.cend(), match, variableRegex)) {
        // 添加匹配前的内容
        tempResult.append(startPos, match[0].first);
        
        // match[0]是完整匹配，如{{ name }}
        // match[1]是捕获组，如name
        std::string key = match[1].str();
        std::string replacement = "";
        
        // 查找变量值，如果不存在则保持原样
        auto it = variables.find(key);
        if (it != variables.end()) {
            replacement = it->second;
        } else {
            // 变量不存在，保留原模板标记
            replacement = match[0].str();
        }
        
        // 添加替换后的内容
        tempResult.append(replacement);
        
        // 更新搜索位置
        startPos = match[0].second;
        searchStart = startPos;
    }
    
    // 添加最后一个匹配后的内容
    tempResult.append(startPos, result.cend());
    
    return tempResult;
}

// 从文件加载模板
bool TemplateEngine::loadTemplateFromFile(const std::string& filePath) {
    // 使用二进制模式打开文件，确保UTF-8编码正确处理
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开模板文件：" << filePath << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();     //重定向
    templateContent = buffer.str();
    file.close();
    
    return true;
}