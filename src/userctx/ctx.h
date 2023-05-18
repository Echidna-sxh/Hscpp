#pragma once
#include <memory>
namespace Echidna
{
    class UserCtx
    {
    public:
        virtual ~UserCtx();
    };

    using CtxPtr = std::shared_ptr<UserCtx>;

}