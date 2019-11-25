#include <vcos/thread.h>

#include "common/locator-getters.hpp"
#include "common/thread.hpp"

using namespace vc;

vcKernelPid vcThreadCreate(vcThread *          aThread,
                           char *              aStack,
                           int                 aStackSize,
                           char                aPriority,
                           int                 aFlags,
                           vcThreadHandlerFunc aFunction,
                           void *              aArg,
                           const char *        aName)
{
    return (static_cast<Thread *>(aThread))->Create(aStack, aStackSize, aPriority, aFlags, aFunction, aArg, aName);
}

void vcThreadSchedulerRun(void)
{
    Instance &instance = Instance::Get();
    instance.Get<ThreadScheduler>().Run();
}

void vcThreadTaskExit(void)
{
    Instance &instance = Instance::Get();
    instance.Get<ThreadScheduler>().TaskExit();
}

int vcThreadPidIsValid(vcKernelPid aPid)
{
    return ((KERNEL_PID_FIRST <= aPid) && (aPid <= KERNEL_PID_LAST));
}
