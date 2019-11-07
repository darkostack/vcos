#include "vcos-core-config.h"

#include <vcos/instance.h>

#include "common/instance.hpp"

using namespace vc;

vcInstance *vcInstanceInitSingle(void)
{
    return &Instance::InitSingle();
}

bool vcInstanceIsInitialized(vcInstance *aInstance)
{
    Instance &instance = *static_cast<Instance *>(aInstance);
    return instance.IsInitialized();
}

void vcInstanceFinalize(vcInstance *aInstance)
{
    Instance &instance = *static_cast<Instance *>(aInstance);
    instance.Finalize();       
}

void vcInstanceReset(vcInstance *aInstance)
{
    Instance &instance = *static_cast<Instance *>(aInstance);
    instance.Reset();
}

const char *vcGetVersionString(void)
{
    static const char sVersion[] = PACKAGE_NAME "/" PACKAGE_VERSION "; " VCOS_CONFIG_PLATFORM_INFO
#if defined(__DATE__)
                                                "; " __DATE__ " " __TIME__
#endif
    ; // Trailing semicolon to end statement.
    return sVersion;
}
