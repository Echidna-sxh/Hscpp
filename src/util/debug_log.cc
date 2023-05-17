#include "debug_log.h"
#include <cstdlib>

#include <iostream>

namespace Echidna
{
    inline static void Yellow(const std::string& msg) {std::cout <<"\033[1;33m" + msg + "\033[0m" << std::endl;} 
    inline static void Green(const std::string& msg) {std::cout <<"\033[0;32m" + msg + "\033[0m" << std::endl;}
    inline static void Red(const std::string& msg) {std::cout <<"\033[0;31m" + msg + "\033[0m" << std::endl;}
    inline static void Blue(const std::string& msg) {std::cout <<"\033[0;34m" + msg + "\033[0m" << std::endl;}
    inline static void LightRed(const std::string& msg) {std::cout <<"\033[1;31m" + msg + "\033[0m" << std::endl;}

    
    std::map<std::string, bool> DebugLogger::bool_map{
        {"True", true},
        {"TRUE", true},
        {"true", true},
        {"T", true},
        {"yes", true},
        {"YES", true},
        {"Y", true},
        {"Yes", true},
        {"1", true},

        {"False", false},
        {"FALSE", false},
        {"false", false},
        {"F", false},
        {"no", false},
        {"NO", false},
        {"No", false},
        {"N", false},
        {"0", false}
    };

    DebugLogger DLogger;

    DebugLogger::DebugLogger()
    {
        char* enable_log = getenv("HSCPP_DLOG_BOOL");
        
        if(!enable_log)
        {
            enabled = false;
        }
        else
        {
            std::string log_bool(enable_log);

            if(bool_map.find(log_bool) != bool_map.end())
            {
                enabled = bool_map[log_bool];
            }
            else
                enabled = false;
            
        }
        
    }

    void DebugLogger::DLog(LogType type, const std::string& msg)
    {
        
        if(!enabled)
        {
            return;
        }

        switch(type)
        {
            case(LogType::Error):
                LightRed("<Error>: " + msg);
                break;
            case(LogType::Warning):
                Red("<Warning>: " + msg);
                break;
            case(LogType::Notice):
                Blue("<Notice>: " + msg);
                break;
            case(LogType::Info):
                Yellow("<Info>: " + msg);
                break;
        }
        
    }
}