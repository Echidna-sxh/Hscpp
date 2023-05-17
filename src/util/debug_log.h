#pragma once
#include <string>
#include <map>

namespace Echidna
{

enum class LogType
{
    Error,
    Warning,
    Notice,
    Info
};

class DebugLogger
{
public:
    DebugLogger();
    void DLog(LogType type, const std::string& msg);

private:
    bool enabled;
    static std::map<std::string, bool> bool_map;
};

extern DebugLogger DLogger;
}

