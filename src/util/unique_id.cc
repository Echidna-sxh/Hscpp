#include "unique_id.h"
#include <mutex>
#include <random>
#include <limits>

namespace Echidna
{
    std::mutex mtx;

    unsigned int RandUint()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<unsigned int> dis(0, std::numeric_limits<unsigned int>::max() - 1);
        return dis(gen);
    }

    uint32_t UniqueIdGen::GetID()
    {
        auto id = RandUint();
        mtx.lock();
        while (id_set.find(id) != id_set.end())
        {
            id = RandUint();
        }
        id_set.emplace(id);
        mtx.unlock();
        return id;
    }

    bool UniqueIdGen::SetID(unsigned int id)
    {
        if (id == std::numeric_limits<unsigned int>::max())
        {
            return false;
        }
        mtx.lock();
        if (id_set.find(id) != id_set.end())
        {
            mtx.unlock();
            return false;
        }
        else
        {
            id_set.emplace(id);
            mtx.unlock();
            return true;
        }
    }

    UniqueIdGen IdGenerator;
}