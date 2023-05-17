#include "pattern.h"
namespace Echidna
{

Pattern::Pattern(const char* expr)
:expression(expr)
{
}

Pattern::Pattern(const std::string& expr)
:expression(expr)
{
}

Pattern::~Pattern(){}

const std::string& Pattern::Get()
{
    return expression;
}

}