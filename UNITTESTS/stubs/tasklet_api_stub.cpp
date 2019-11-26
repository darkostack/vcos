#include "net/tasklet.hpp"

#include "common/instance.hpp"

using namespace vc;

__attribute__((weak)) void vcNetTaskletsSignalPending(vcInstance *aInstance)
{
    (void) aInstance;
}
