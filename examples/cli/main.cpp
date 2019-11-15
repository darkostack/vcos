#include <stdio.h>
#include <assert.h>

#include <vcos/cli.h>

#include "common/instance.hpp"

namespace vc {

int mainApp(void *aArg)
{
    Instance &instance = *static_cast<Instance *>(aArg);

    assert(instance.IsInitialized());

    vcCliUartInit((vcInstance *)&instance);

    vcCliUartRun();

    /* Should not reach here */
    assert(0);

    return 0;
}

} // namespace vc
