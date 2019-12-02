#ifndef LOCATOR_GETTERS_HPP
#define LOCATOR_GETTERS_HPP

#include "vcos-core-config.h"

#include "common/instance.hpp"
#include "common/locator.hpp"

namespace vc {

template <> inline Instance &InstanceLocator::Get(void) const
{
    return GetInstance();
}

template <typename Type> inline Type &InstanceLocator::Get(void) const
{
    // This method uses the `Instance` template method `Get<Type>`
    // to get to the given `Type` from the single instance.
    return GetInstance().Get<Type>();
}

} // namespace vc

#endif /* LOCATOR_GETTERS_HPP */
