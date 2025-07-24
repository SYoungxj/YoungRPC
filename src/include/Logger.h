#ifndef __Logger_H__
#define __Logger_H__

#include "LockQueue.h"
#include <string>

using std::string;

enum LogLevel
{
    INFO, // 普通信息
    ERROR, // 错误信息
};


// Mprpc 框架提供的日志系统
class Logger : public NonCopyable
{
public:
    static Logger * GetInstance();
    // 设置日志级别
    void SetLogLevel(LogLevel level);
    // 写日志
    void Log(string msg);

private:
    class AutoRelease
    {
    public:
        AutoRelease() = default;
        ~AutoRelease()
        {
            if(nullptr != pInstance_)
            {
                delete pInstance_;
                pInstance_ = nullptr;
            }
        }
    };

    Logger();
    ~Logger();

    static Logger * pInstance_;
    int logLevel_; // 记录日志级别
    LockQueue<string> lockQue_; // 日志缓冲队列
};

// 定义日志宏
// 支持用户自定义可变信息
// 类似于 printf
#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        Logger * logger = Logger::GetInstance(); \
        logger->SetLogLevel(LogLevel::INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger->Log(c);  \ 
    } while(0);

#define LOG_ERROR(logmsgformat, ...) \
    do \
    {  \
        Logger * logger = Logger::GetInstance(); \
        logger->SetLogLevel(LogLevel::ERROR); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger->Log(c);  \ 
    } while(0);

#endif
