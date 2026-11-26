#pragma once
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<ctime>

class Logger{
    private:
    Logger(){
    }

    ~Logger(){
    }
    
    public:

    static Logger& getInstance(){
        static Logger instance;
        return instance;
    }

    void Log(const char* level,const char* file,int line,const std::string& message){
        time_t now = time(0);
        tm* t = localtime(&now);
        char timeStr[80];
        strftime(timeStr,sizeof(timeStr),"%Y-%m-%d %H:%M:%S",t);

        std::ostringstream oss;
        oss << "[" << timeStr << "] [" << level << "] [" << file << ":" << line << "] " << message;

        std::cout << oss.str() << std::endl;
        }

};




#define LOG_DEBUG(...)do{\
    std::ostringstream oss;\
    oss << __VA_ARGS__;\
    Logger::getInstance().Log("DEBUG",__FILE__,__LINE__,oss.str());\
}while(0)

#define LOG_INFO(...)do{\
    std::ostringstream oss;\
    oss << __VA_ARGS__;\
    Logger::getInstance().Log("INFO",__FILE__,__LINE__,oss.str());\
}while(0)

#define LOG_WARN(...)do{\
    std::ostringstream oss;\
    oss << __VA_ARGS__;\
    Logger::getInstance().Log("WARN",__FILE__,__LINE__,oss.str());\
}while(0)

#define LOG_ERROR(...)do{\
    std::ostringstream oss;\
    oss << __VA_ARGS__;\
    Logger::getInstance().Log("ERROR",__FILE__,__LINE__,oss.str());\
}while(0)

#define LOG_FATAL(...)do{\
    std::ostringstream oss;\
    oss << __VA_ARGS__;\
    Logger::getInstance().Log("FATAL",__FILE__,__LINE__,oss.str());\
}while(0)

#define LOG_UNKNOWN(...)do{\
    std::ostringstream oss;\
    oss << __VA_ARGS__;\
    Logger::getInstance().Log("UNKNOWN",__FILE__,__LINE__,oss.str());\
}while(0)


