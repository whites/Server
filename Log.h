#ifndef LOG_H_
#define LOG_H_

#include <cstdarg>
#include <stdio.h>
#include <string>

class Log
{
private:
    int logLevel_;
    Log(){};
    ~Log(){};

public:
    static Log* GetInstance()  // singleton
    {
        static Log netMng;
        return &netMng;
    }

    void logData(int log_level, std::string filename, int line, const char* format, ... )
    {
        char msg[1024] = {0};
        std::va_list args;
        va_start(args, format);
        vsnprintf(msg, 1024, format, args);
        va_end(args);

        printf("[%s:%d]%s\n", filename.c_str(), line, msg);
    }
};


#define logger Log::GetInstance()

#define DEBUG(format, args...) logger->logData(1, __FILE__, __LINE__, ,format, ##args)
#define INFO(format, args...) logger->logData(2, __FILE__, __LINE__, format, ##args)
#define ERROR(format, args...) logger->logData(3, __FILE__, __LINE__, format, ##args)

#endif /* LOG_H_ */