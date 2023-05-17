#pragma once
#include <set>
#include <stdint.h>
namespace Echidna
{
class UniqueIdGen
{
public:
    uint32_t GetID();
    bool SetID(unsigned int id);

private:
    std::set<uint32_t> id_set;
};
extern UniqueIdGen IdGenerator;
}