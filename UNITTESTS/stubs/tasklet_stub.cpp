#include <vcos/instance.h>

using namespace vc;

__attribute__((weak)) void vcNetTaskletsSignalPending(vcInstance *aInstance)
{
    (void) aInstance;
}
