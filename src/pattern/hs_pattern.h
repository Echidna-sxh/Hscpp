#pragma once
#include "pattern.h"
#include <hs/hs.h>
#include <memory>
#include "ctx.h"
#include <limits>

#define AUTOID std::numeric_limits<unsigned int>::max()

namespace Echidna
{

using ExFlagPtr = std::shared_ptr<hs_expr_ext_t>;

constexpr static unsigned int EDIT_DISTANCE_MAX = 1000;
constexpr static unsigned int HAMMING_DISTANCE_MAX = 1000;

class Hs_Pattern :public Pattern
{
public:
    Hs_Pattern() = delete;
    Hs_Pattern(const std::string&, UserCtx* ctx = nullptr);
    Hs_Pattern(const std::string&, uint32_t id, UserCtx* ctx = nullptr);
    Hs_Pattern(const std::string&, uint32_t id, uint32_t flag, UserCtx* ctx = nullptr);
    const std::string& Get();
    const UserCtx* GetUerCtx(){return userctx.get();}
    uint32_t GetId(){return id;}
    uint32_t GetFlag(){return flag;}
    ExFlagPtr GetExFlag(){return ex_flag;};
    enum class FLAG
    {
        caseless = HS_FLAG_CASELESS,
        dotall = HS_FLAG_DOTALL,
        multiline = HS_FLAG_MULTILINE,
        singlematch = HS_FLAG_SINGLEMATCH,
        allowempty = HS_FLAG_ALLOWEMPTY,
        utf_8 = HS_FLAG_UTF8,
        UCP = HS_FLAG_UCP,
        prefilter = HS_FLAG_PREFILTER,
        leftmost = HS_FLAG_SOM_LEFTMOST,
        combination = HS_FLAG_COMBINATION,
        quiet = HS_FLAG_QUIET
    };

    enum class EX_FLAG
    {
        min_offset = HS_EXT_FLAG_MIN_OFFSET,
        max_offset = HS_EXT_FLAG_MAX_OFFSET,
        min_length = HS_EXT_FLAG_MIN_LENGTH,
        edit_distance = HS_EXT_FLAG_EDIT_DISTANCE,
        hamming_distance = HS_EXT_FLAG_HAMMING_DISTANCE
    };

    void Addflag(FLAG uflag);
    void Removeflag(FLAG uflag);
    void Set_min_offset(unsigned long long);
    void Set_max_offset(unsigned long long);
    void Set_min_length(unsigned long long);
    void Set_edit_distance(unsigned int);
    void Set_hamming_distance(unsigned int);
    
private:
    uint32_t id;
    uint32_t flag;
    ExFlagPtr ex_flag;
    CtxPtr userctx;
};

using HsPatPtr = std::shared_ptr<Hs_Pattern>;

}