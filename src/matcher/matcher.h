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
    /**
     * it is called when the matcher hit. The match result will be passed as params.
     *
     * @param id
     *      the pattern id. If you did not pass in one, it will be a random number.  
     * @param from
     *      Only when Hs_Pattern::FLAG::leftmost and HsMatcher::LeftMatchFlag is set, 
     *      it will be the left offset of the match.Otherwise it will be zero.
     * @param to
     *      It is the right offset of the match.
     * @param match_ctx
     *      it is the user-context pass in when you call Match() or SafeMatch(). 
     * @param pat_ctx
     *      it is the user-context of the pattern that the matcher hit.See @ref Hs_Pattern.
     */     
    using MatchCb = std::function<int(unsigned int id, unsigned long long from, unsigned long long to, const UserCtx *match_ctx, const UserCtx *pat_ctx)>;

    class Matcher
    {
    public:
        virtual ~Matcher() = 0;
        virtual void RegisteCb(MatchCb) = 0;
        virtual void Match(const std::string &data, UserCtx *ctx = nullptr) = 0;

    protected:
        std::vector<PatPtr> patterns;
    };

    using MatcherPtr = std::shared_ptr<Matcher>;

}