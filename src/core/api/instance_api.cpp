#include <vcos/instance.h>

#include "common/instance.hpp"

using namespace vc;

vcInstance *vcInstanceInit(void)
{
    return &Instance::Init();
}

bool vcInstanceIsInitialized(vcInstance *aInstance)
{
    Instance &instance = *static_cast<Instance *>(aInstance);
    return instance.IsInitialized();
}

vcInstance *vcInstanceGet(void)
{
    return &Instance::Get();
}

const char *vcInstanceGetVersionString(void)
{
    static const char sVersion[] =
        VCOS_CONFIG_PACKAGE_NAME "/" VCOS_CONFIG_PACKAGE_VERSION "; " VCOS_CONFIG_PLATFORM_INFO
#if defined(__DATE__)
                                 "; " __DATE__ " " __TIME__
#endif
        ; // Trailing semicolon to end statement.
    return sVersion;
}
