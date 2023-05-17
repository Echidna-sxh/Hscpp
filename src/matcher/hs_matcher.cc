#include "hs_matcher.h"
#include <algorithm>
#include "debug_log.h"

namespace Echidna
{

struct ScanCtx
{
    MatchCb cb_handler;
    CtxPtr ctx;
    std::vector<PatPtr>* patterns;
};

Scratch::Scratch(hs_database_t* db)
{
    auto res = hs_alloc_scratch(db, &prototype.scr);
    prototype.in_use = false;
}

Scratch::~Scratch()
{
    hs_free_scratch(prototype.scr);
    for(auto&& i : ScrPool)
    {
        hs_free_scratch(i.scr);
    }
}

hs_scratch_t* Scratch::GetScratch()
{
    return prototype.scr;
}

hs_scratch_t* Scratch::GetSafeScratch()
{
    hs_scratch_t* res = nullptr;
    for(auto&& i : ScrPool)
    {
        if(!i.in_use)
        {
            i.mtx.lock();
            if(!i.in_use)
            {
                res = i.scr;
                i.in_use = true;
                i.mtx.unlock();
                break;
            }
            i.mtx.unlock();
            continue;
        }
    }

    if(!res)
    {
        PoolMtx.lock();
        ScratchData dolly;
        hs_clone_scratch(prototype.scr, &dolly.scr);
        dolly.in_use = true;
        res = dolly.scr;
        ScrPool.push_back(dolly);
        PoolMtx.unlock();
    }
    return res;
}

void Scratch::Release(hs_scratch_t* scr)
{
    for(auto&& i : ScrPool)
    {
        if(i.scr == scr)
        {
            i.mtx.lock();
            i.in_use = false;
            i.mtx.unlock();
            break;
        }
    }
}

HsMatcher::HsMatcher()
:db(nullptr),
updated(true),
compile_mode(HS_MODE_BLOCK),
cb_handler(defaultcb){}

HsMatcher::~HsMatcher()
{
    if(db)
    {
        hs_free_database(db);
    }
}

void HsMatcher::push_back(Hs_Pattern& pat)
{
    auto patptr = std::make_shared<Hs_Pattern>(pat);
    patterns.push_back(patptr);
    updated = true;
}

void HsMatcher::push_back(const std::string& pat)
{
    auto patptr = std::make_shared<Hs_Pattern>(pat);
    patterns.push_back(patptr);
    updated = true;
}

void HsMatcher::erase(Hs_Pattern& pat, std::function<int(Hs_Pattern&, Hs_Pattern&)> equal)
{
    std::remove_if(patterns.begin(), patterns.end(),
        [&](PatPtr patptr){
            return equal(*dynamic_cast<Hs_Pattern*>(patptr.get()), pat);
        });
    updated = true;
}

void HsMatcher::RegisteCb(MatchCb cb)
{
    cb_handler = cb;
}

PatPtr HsMatcher::find(uint32_t id)
{
    for(auto&& i: patterns)
    {
        if(dynamic_cast<Hs_Pattern*>(i.get())->GetId() == id)
        {
            return i;
        }
    }
    return nullptr;
}

void HsMatcher::clear()
{
    updated = true;
    hs_free_database(db);
    db = nullptr;
    scratch.reset();
    patterns.clear();
}

void HsMatcher::SetMode(MatchMode umode)
{
    switch(umode)
    {
        case(MatchMode::block):
            compile_mode |= HS_MODE_BLOCK;
            break;
        case(MatchMode::stream):
            compile_mode |= HS_MODE_STREAM;
            break;
        case(MatchMode::vector):
            compile_mode |= HS_MODE_VECTORED;
            break;
        default:
            compile_mode |= HS_MODE_BLOCK;
            break;
    }
}

void HsMatcher::SetMatchFlag(LeftMatchFlag uflag)
{
    switch(uflag)
    {
        case(LeftMatchFlag::large):
            compile_mode |= HS_MODE_SOM_HORIZON_LARGE;
            break;
        case(LeftMatchFlag::medium):
            compile_mode |= HS_MODE_SOM_HORIZON_MEDIUM;
            break;
        case(LeftMatchFlag::small):
            compile_mode |= HS_MODE_SOM_HORIZON_SMALL;
            break;
        case(LeftMatchFlag::none):
            break;
        default:
            break;
    }
}

int HsMatcher::compile()
{
    scratch.reset();
    if(db)
    {
        hs_free_database(db);
        db = nullptr;
    }
    
    const char** expressions = new const char* [patterns.size()];
    unsigned int* pflags = new unsigned int[patterns.size()];
    unsigned int* ids = new unsigned int[patterns.size()];
    hs_expr_ext_t** ext = new hs_expr_ext_t*[patterns.size()];
    hs_compile_error_t* error = nullptr; 

    for(int i = 0; i < patterns.size(); i++)
    {
        Hs_Pattern* pat = dynamic_cast<Hs_Pattern*>(patterns[i].get());
        expressions[i] = pat->Get().data();
        pflags[i] = pat->GetFlag();
        ids[i] = pat->GetId();
        ext[i] = pat->GetExFlag().get();
    }

    auto ret = hs_compile_ext_multi(expressions, pflags, ids, ext, patterns.size(), compile_mode, nullptr, &db, &error);
    if(ret != HS_SUCCESS)
    {
        DLogger.DLog(LogType::Warning, "hs compile error! error no is" + std::to_string(ret) + " -> " + error->message);
    }
    
    scratch = std::make_shared<Scratch>(db);
    hs_free_compile_error(error);
    delete[] expressions;
    delete[] pflags;
    delete[] ids;
    delete[] ext;
    return ret;
}

void HsMatcher::Match(const std::string& data, CtxPtr ctx)
{
    if(patterns.size() == 0)
    {
        DLogger.DLog(LogType::Notice, "The matcher is empty!");
        return;
    }

    if(!db || updated)
    {
        DLogger.DLog(LogType::Notice, "generate database automatically!");
        auto ret = compile();
        if(ret != HS_SUCCESS)
        {
            DLogger.DLog(LogType::Error, "db compile update failed, match failed!");
            return;
        }
        updated = false;
    }

    auto scr = scratch->GetScratch();
    ScanCtx scanctx{this->cb_handler, ctx, &this->patterns};
    auto res = hs_scan(db, data.data(), data.size(), 0, scr, HsMatcher::callback, &scanctx);
}

void HsMatcher::SafeMatch(const std::string& data, CtxPtr ctx)
{
    if(!db || updated)
    {
        DLogger.DLog(LogType::Warning, "generate database automatically!");
        compile();
    }

    auto scr = scratch->GetSafeScratch();
    ScanCtx scanctx{this->cb_handler, ctx, &this->patterns};
    auto res = hs_scan(db, data.data(), data.size(), 0, scr, HsMatcher::callback, &scanctx);
}

int HsMatcher::callback(unsigned int id, unsigned long long from, unsigned long long to, unsigned int flags, void *context)
{
    ScanCtx* scanctx = reinterpret_cast<ScanCtx*>(context);
    auto patterns = scanctx->patterns;
    PatPtr target = nullptr;
    for(auto&& i: *patterns)
    {
        if(dynamic_cast<Hs_Pattern*>(i.get())->GetId() == id)
        {
            target = i;
            break;
        }
    }
    if(!target)
    {
        DLogger.DLog(LogType::Warning, "no pattern matched but matcher hit, check mutithread!!!");
        return 0;
    }
    return scanctx->cb_handler(id, from, to, scanctx->ctx.get(), dynamic_cast<Hs_Pattern*>(target.get())->GetUerCtx());
}

}