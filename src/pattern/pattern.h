#pragma once
#include <string>
#include <memory>
namespace Echidna
{

class Pattern
{
public:
    Pattern() = delete;
    Pattern(const char*);
    Pattern(const std::string&);
    virtual ~Pattern() = 0;
    virtual const std::string& Get();
protected:
    const std::string expression;
};

using PatPtr = std::shared_ptr<Pattern>;
}