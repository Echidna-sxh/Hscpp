#include "hs_pattern.h"
#include "unique_id.h"
#include "debug_log.h"
#include <string.h>

namespace Echidna
{
    Hs_Pattern::Hs_Pattern(const std::string &pat, UserCtx *ctx)
    :Pattern(pat)
    {
        id = IdGenerator.GetID();
        flag = 0;
        userctx = std::make_shared<UserCtx>(*ctx);
    }

    Hs_Pattern::Hs_Pattern(const std::string &pat, uint32_t uid, UserCtx *ctx)
    :Pattern(pat)
    {
        if (uid == AUTOID)
        {
            id = IdGenerator.GetID();
        }
        else
        {
            if (!IdGenerator.SetID(uid))
            {
                DLogger.DLog(LogType::Error, "to large or duplicate id:" + std::to_string(uid) + " -> " + pat);
                exit(0);
            }
            id = uid;
        }
        flag = 0;
        userctx = std::make_shared<UserCtx>(*ctx);
    }

    Hs_Pattern::Hs_Pattern(const std::string &pat, uint32_t uid, uint32_t uflag, UserCtx *ctx)
    :Pattern(pat)
    {
        if (uid == AUTOID)
        {
            id = IdGenerator.GetID();
        }
        else
        {
            if (!IdGenerator.SetID(uid))
            {
                DLogger.DLog(LogType::Error, "too large id or duplicate id:" + std::to_string(uid) + " -> " + pat);
                exit(0);
            }
            id = uid;
        }
        flag = uflag;
        userctx = std::make_shared<UserCtx>(*ctx);
    }

    const std::string& Hs_Pattern::Get()
    {
        return this->expression;
    }

    void Hs_Pattern::Addflag(FLAG uflag)
    {
        flag |= static_cast<int>(uflag);
    }

    void Hs_Pattern::Removeflag(FLAG uflag)
    {
        flag &= ~static_cast<int>(uflag);
    }

    void InitExFlag(std::shared_ptr<hs_expr_ext_t> ex_flag)
    {
        ex_flag = std::make_shared<hs_expr_ext_t>();
        ex_flag->flags = 0;
        ex_flag->max_offset = 0;
        ex_flag->min_length = 0;
        ex_flag->min_offset = 0;
        ex_flag->edit_distance = 0;
        ex_flag->hamming_distance = 0;
    }

    void Hs_Pattern::Set_min_offset(unsigned long long offset)
    {
        if(!ex_flag)
        {
            InitExFlag(ex_flag);
        }
        ex_flag->min_offset = offset;
        ex_flag->flags |= HS_EXT_FLAG_MIN_OFFSET;
    }

    void Hs_Pattern::Set_max_offset(unsigned long long offset) 
    {
        if(!ex_flag)
        {
            InitExFlag(ex_flag);
        }
        ex_flag->max_offset = offset;
        ex_flag->flags |= HS_EXT_FLAG_MAX_OFFSET;
    }

    void Hs_Pattern::Set_min_length(unsigned long long min_length)
    {
        if(!ex_flag)
        {
            InitExFlag(ex_flag);
        }
        ex_flag->min_length = min_length;
        ex_flag->flags |= HS_EXT_FLAG_MIN_LENGTH;
    }

    void Hs_Pattern::Set_edit_distance(unsigned int edit_distance)
    {
        if(!ex_flag)
        {
            InitExFlag(ex_flag);
        }
        if(edit_distance > EDIT_DISTANCE_MAX)
        {
            DLogger.DLog(LogType::Warning, " performance may be low, due to large edit_distance:" + std::to_string(edit_distance));
        }
        ex_flag->edit_distance = edit_distance;
        ex_flag->flags |= HS_EXT_FLAG_EDIT_DISTANCE;
    }

    void Hs_Pattern::Set_hamming_distance(unsigned int hamming_distance)
    {
        if(!ex_flag)
        {
            InitExFlag(ex_flag);
        }
        if(hamming_distance > EDIT_DISTANCE_MAX)
        {
            DLogger.DLog(LogType::Warning, " performance may be low, due to large hamming_distance:" + std::to_string(hamming_distance));
        }
        ex_flag->hamming_distance = hamming_distance;
        ex_flag->flags |= HS_EXT_FLAG_HAMMING_DISTANCE;
    }

}