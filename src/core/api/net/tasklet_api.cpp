#include "net/tasklet.hpp"

#include "common/instance.hpp"
#include "common/code_utils.hpp"

using namespace vc;

void vcNetTaskletProcess(vcInstance *aInstance)
{
    Instance &instance = *static_cast<Instance *>(aInstance);

    VerifyOrExit(vcInstanceIsInitialized(aInstance));

    instance.Get<Net::TaskletScheduler>().ProcessQueuedTasklets();

exit:
    return;
}

bool vcNetTaskletsArePending(vcInstance *aInstance)
{
    bool ret = false;
    Instance &instance = *static_cast<Instance *>(aInstance);

    VerifyOrExit(vcInstanceIsInitialized(aInstance));

    ret = instance.Get<Net::TaskletScheduler>().AreTaskletsPending();

exit:
    return ret;
}

__attribute__((weak)) void vcNetTaskletsSignalPending(vcInstance *aInstance)
{
    (void) aInstance;
}
