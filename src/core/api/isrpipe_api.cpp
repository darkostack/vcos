#include <vcos/isrpipe.h>

#include "common/instance.hpp"
#include "utils/isrpipe.hpp"

using namespace vc;

int vcIsrpipeUartWriteOne(vcInstance *aInstance, char aChar)
{
    int res = -1;
    Instance *instance = static_cast<Instance *>(aInstance);
    VerifyOrExit(vcInstanceIsInitialized(aInstance));
    res = instance->Get<Utils::IsrpipeUart>().WriteOne(aChar);

exit:
    return res;
}


int vcIsrpipeUartRead(vcInstance *aInstance, char *aBuf, size_t aCount)
{
    int res = -1;
    Instance *instance = static_cast<Instance *>(aInstance);
    VerifyOrExit(vcInstanceIsInitialized(aInstance));
    res = instance->Get<Utils::IsrpipeUart>().Read(aBuf, aCount);

exit:
    return res;
}
