#pragma once
#include "matcher.h"
#include <hs/hs.h>
#include <vector>
#include <functional>
#include <mutex>
#include <memory>
namespace Echidna
{

using HandlerFunc = std::function<int(unsigned int id, unsigned long long from, unsigned long long to, unsigned int flags, void *context)>;

static MatchCb defaultcb = [](unsigned int, unsigned long long from, unsigned long long to, const UserCtx* m_ctx, const UserCtx* p_ctx)->int
    {
        DLogger.DLog(LogType::Info, "Matcher hit at <pos> " + std::to_string(to));
        return 0;
    };

inline static int Equal(Hs_Pattern& pat1, Hs_Pattern& pat2)
{
    return pat1.GetId() == pat2.GetId();
}

struct ScratchData
{
    hs_scratch_t* scr;
    std::mutex mtx;
    bool in_use;
    ScratchData()
    {
        scr = nullptr;
        in_use = false;
    }
    ScratchData(const ScratchData& data)
    {
        scr = data.scr;
        in_use = data.in_use;
    }
};

class Scratch
{
public:
    Scratch() = delete;
    Scratch(hs_database_t* db);
    hs_scratch_t* GetSafeScratch();
    hs_scratch_t* GetScratch();
    void Release(hs_scratch_t* scr);
    ~Scratch();
private:
    ScratchData prototype;
    std::mutex PoolMtx;
    std::vector<ScratchData> ScrPool;
};

class HsMatcher: public Matcher
{
public:
    HsMatcher();
    ~HsMatcher();
    enum class MatchMode
    {
        block = HS_MODE_BLOCK,
        stream = HS_MODE_STREAM,
        vector = HS_MODE_VECTORED
    }; 
    enum class LeftMatchFlag
    {
        large = HS_MODE_SOM_HORIZON_LARGE,
        medium = HS_MODE_SOM_HORIZON_MEDIUM,
        small = HS_MODE_SOM_HORIZON_SMALL,
        none
    };
    void push_back(Hs_Pattern&);
    void push_back(const std::string&);
    void erase(Hs_Pattern& pat, std::function<int(Hs_Pattern&, Hs_Pattern&)> equal = Equal);
    void clear();
    PatPtr find(uint32_t id);
    void RegisteCb(MatchCb cb = defaultcb);
    void SetMode(MatchMode);
    void SetMatchFlag(LeftMatchFlag);
    int compile(); 
    void Match(const std::string& data, CtxPtr ctx = nullptr);
    void SafeMatch(const std::string& data, CtxPtr ctx = nullptr);

private:
    bool updated;
    uint32_t compile_mode;
    hs_database_t* db;
    MatchCb cb_handler;
    static int callback(unsigned int id, unsigned long long from, unsigned long long to, unsigned int flags, void *context);
    std::shared_ptr<Scratch> scratch;

};


}