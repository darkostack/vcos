#ifndef LOCATOR_HPP
#define LOCATOR_HPP

#include "vcos-core-config.h"

#include <stdint.h>

namespace vc {

class Instance;

extern uint64_t gInstanceRaw[];

class InstanceLocator
{
public:
    Instance &GetInstance(void) const { return *reinterpret_cast<Instance *>(&gInstanceRaw); }

    template <typename Type> inline Type &Get(void) const; // Implemented in `locator-getters.hpp`.
};

} // namespace vc

#endif /* LOCATOR_HPP */
