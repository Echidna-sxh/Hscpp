#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "hs_pattern.h"
#include "ctx.h"
#include "debug_log.h"

namespace Echidna
{

using MatchCb = std::function<int(unsigned int id, unsigned long long from, unsigned long long to, const UserCtx* match_ctx, const UserCtx* pat_ctx)>;

class Matcher 
{
public:
    virtual ~Matcher() = 0;
    virtual void RegisteCb(MatchCb) = 0;
    virtual void Match(const std::string& data, CtxPtr ctx = nullptr) = 0;

protected:
    std::vector<PatPtr> patterns;

};

using MatcherPtr = std::shared_ptr<Matcher>;

}