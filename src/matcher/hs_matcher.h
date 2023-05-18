/* hs_matcher.h - main header for GPL licensing
 * Copyright (C) 2023 John Doe
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

/**
 * @file
 * @brief a C++ interface implementation for hyperscan matcher.
 *
 * Hyperscan is a high speed regular expression engine.
 *
 * This header contains all interface hs_matcher provides.
 * Just include this one file if you need to use this matcher.
 */

#include "matcher.h"
#include <hs/hs.h>
#include <vector>
#include <functional>
#include <mutex>
#include <memory>

namespace Echidna
{
    /**
     * The default callback funtion.It will be called if the matcher hits and there no callback passed in.
     */
    static MatchCb defaultcb = [](unsigned int, unsigned long long from, unsigned long long to, const UserCtx *m_ctx, const UserCtx *p_ctx) -> int
    {
        DLogger.DLog(LogType::Info, "Matcher hit at <pos> " + std::to_string(to));
        return 0;
    };

    /**
     * The default compare funtion between patterns. It will be called if @ref erase() called and there no 'equal' function param passed in.
     */
    inline static int Equal(Hs_Pattern &pat1, Hs_Pattern &pat2)
    {
        return pat1.GetId() == pat2.GetId();
    }

    /**
     * it contains the scratch data that hyperscan needs, generally users don't need to care it.
     */
    struct ScratchData
    {
        hs_scratch_t *scr;
        std::mutex mtx;
        bool in_use;
        ScratchData()
        {
            scr = nullptr;
            in_use = false;
        }
        ScratchData(const ScratchData &data)
        {
            scr = data.scr;
            in_use = data.in_use;
        }
    };

    /**
     * it implement a thread-safe scratch, generally users don't need to care it.
     */
    class Scratch
    {
    public:
        Scratch() = delete;
        Scratch(hs_database_t *db);
        hs_scratch_t *GetSafeScratch();
        hs_scratch_t *GetScratch();
        void Release(hs_scratch_t *scr);
        ~Scratch();

    private:
        ScratchData prototype;
        std::mutex PoolMtx;
        std::vector<ScratchData> ScrPool;
    };

    /**
     * it implement the hyperscan matcher, generally users only need to use the interfaces inside.
     */
    class HsMatcher : public Matcher
    {
    public:
        HsMatcher();
        ~HsMatcher();

        enum class MatchMode
        {
            /**
             * Compiler mode flag: Block scan (non-streaming) database.
             */
            block = HS_MODE_BLOCK,

            /**
             * Compiler mode flag: Streaming database.
             */
            stream = HS_MODE_STREAM,

            /**
             * Compiler mode flag: Vectored scanning database.
             */
            vector = HS_MODE_VECTORED
        };

        enum class LeftMatchFlag
        {
            /**
             * Compiler mode flag: use full precision to track start of match offsets in
             * stream state.
             *
             * This mode will use the most stream state per pattern, but will always return
             * an accurate start of match offset regardless of how far back in the past it
             * was found.
             *
             * One of the HsMatcher::LeftMatchFlag modes must be selected to use the @ref
             * Hs_Pattern::FLAG::leftmost expression flag.
             */
            large = HS_MODE_SOM_HORIZON_LARGE,

            /**
             * Compiler mode flag: use medium precision to track start of match offsets in
             * stream state.
             *
             * This mode will use less stream state than @ref HsMatcher::LeftMatchFlag::large and
             * will limit start of match accuracy to offsets within 2^32 bytes of the
             * end of match offset reported.
             *
             * One of the HsMatcher::LeftMatchFlag modes must be selected to use the @ref
             * Hs_Pattern::FLAG::leftmost expression flag.
             */
            medium = HS_MODE_SOM_HORIZON_MEDIUM,

            /**
             * Compiler mode flag: use limited precision to track start of match offsets in
             * stream state.
             *
             * This mode will use less stream state than @ref HsMatcher::LeftMatchFlag::medium and
             * will limit start of match accuracy to offsets within 2^16 bytes of the
             * end of match offset reported.
             *
             * One of the HsMatcher::LeftMatchFlag modes must be selected to use the @ref
             * Hs_Pattern::FLAG::leftmost expression flag.
             */
            small = HS_MODE_SOM_HORIZON_SMALL,

            /**
             * Compiler mode flag: use limited precision to track start of match offsets in
             * stream state.
             *
             * This mode will not get start of match accuracy to offsets.
             */
            none
        };

        /**
         * Add a regex pattern to the matcher.
         *
         * @param pat
         *      the regex expression, type is @ref Hs_Pattern&, and can be implicit constructed
         *      by const char* and std::string.
         */
        void push_back(Hs_Pattern &pat);

        /**
         * Add a regex pattern to the matcher.
         *
         * @param pat
         *      the regex expression, type is @ref std::string, and can be implicit constructed
         *      by const char*.
         */
        void push_back(const std::string &);

        /**
         * Remove a regex pattern to the matcher.
         *
         * @param pat
         *       the pattern object you passed in before.
         * @param equal
         *      a std::function that returns 1 if two patterns equals. The default one is to compare id.
         *
         * tips: if you cannot get the pattern object that you wants to delete, pass in your own id when
         *       generate a pattern object or implement your own equal function.
         *
         */
        void erase(Hs_Pattern &pat, std::function<int(Hs_Pattern &, Hs_Pattern &)> equal = Equal);

        /**
         * Remove a regex pattern to the matcher.
         *
         * @param id
         *       the pattern id you passed in or automically got from the pattern object before.

         * tips: if you cannot get the pattern object that you wants to delete, pass in your own id when
         *       generate a pattern object or implement your own equal function.
         *
         */
        void erase(uint32_t id);

        /**
         * Remove all patterns from the matcher.
         */
        void clear();

        /**
         * find a regex pattern from the matcher.
         *
         * @param id
         *       the pattern id you passed in or automically got from the pattern object before.
         *
         */
        PatPtr find(uint32_t id);

        /**
         * pass in a std::function, and it will be called when the matcher hits.
         *
         * @param cb
         *      the param type is @ref MatchCb, please refer to matcher.h
         *
         */
        void RegisteCb(MatchCb cb = defaultcb);

        /**
         * set hyperscan database mode,
         *
         * @param mode
         *      @ref Echidna::MatchMode
         */
        void SetMode(MatchMode mode);

        /**
         * set hyperscan database whether to match left offset
         *
         * @param flag
         *      @ref Echidna::LeftMatchFlag
         */
        void SetMatchFlag(LeftMatchFlag flag);

        /**
         *  Compile hyperscan database. It will be automatically called.
         *  You can call it manually too.
         */
        int compile();

        /**
         * Match the given string with the matcher. If hit, default or registed callback function
         * will be called.It has almost no performance loss, but it is not thread safe as well as hyperscan
         * itself.
         *
         * @param data
         *      @ref Echidna::LeftMatchFlag
         * @param ctx
         *      it will be passed to the callback function if hit.
         */
        void Match(const std::string &data, UserCtx *ctx = nullptr);

        /**
         * Match the given string with the matcher. If hit, default or registed callback function
         * will be called.It has a little performance loss, but it is thread safe.
         *
         * @param data
         *      @ref Echidna::LeftMatchFlag
         * @param ctx
         *      it will be passed to the callback function if hit.
         */
        void SafeMatch(const std::string &data, UserCtx *ctx = nullptr);

    private:
        bool updated;
        uint32_t compile_mode;
        hs_database_t *db;
        MatchCb cb_handler;
        static int callback(unsigned int id, unsigned long long from, unsigned long long to, unsigned int flags, void *context);
        std::shared_ptr<Scratch> scratch;
    };

}