#include "common/random.hpp"

#include "utils/tinymt32/tinymt32.h"

namespace vc {
namespace Random {

static tinymt32_t sRandom;

void Init(uint32_t aSeed)
{
    tinymt32_init(&sRandom, aSeed);
}

uint32_t GetUint32(void)
{
    return tinymt32_generate_uint32(&sRandom);
}

} // namespace Random
} // namespace vc
