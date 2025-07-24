#include "Logger.h"
#include <iostream>

using std::cout;

Logger * Logger::pInstance_ = nullptr;

Logger::Logger()
{
    thread writeLogThread([&](){
        for(;;)
        {
            // 获取当前的日期，然后取日志信息
            // 并写入相应的日志文件中
            // a+ (追加)
            time_t now = time(nullptr);
            tm * nowtm = localtime(&now);
            
            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year+1900,
                                            nowtm->tm_mon, nowtm->tm_mday);

            FILE * pf = fopen(file_name, "a+");
            if(pf == nullptr)
            {
                cout << "Logger file: " << file_name << "open error!\n"; 
                exit(EXIT_FAILURE);
            }

            string msg = lockQue_.Pop();

            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d => [%s] => ", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec,
                                                                logLevel_ == LogLevel::INFO ? "INFO" : "ERROR");;
            msg.insert(0, time_buf);
            msg.append("\n");

            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });

    // 设置分离线程，守护线程
    writeLogThread.detach();
}

Logger::~Logger()
{

}

Logger * Logger::GetInstance()
{
    if(nullptr == pInstance_)
    {
        pInstance_ = new Logger();
    }

    return pInstance_;
}

void Logger::SetLogLevel(LogLevel level)
{
    logLevel_ = level;
}

// 写日志，把日志信息写入到缓冲区 lock queue
void Logger::Log(string msg)
{
    lockQue_.Push(msg);
}
