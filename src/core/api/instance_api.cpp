#include <vcos/instance.h>

#include "common/instance.hpp"

using namespace vc;

const char *vcGetVersionString(void)
{
    static const char sVersion[] = PACKAGE_NAME "/" PACKAGE_VERSION "; " VCOS_CONFIG_PLATFORM_INFO
#if defined(__DATE__)
                                                "; " __DATE__ " " __TIME__
#endif
    ; // Trailing semicolon to end statement.
    return sVersion;
}
