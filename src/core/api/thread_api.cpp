#include <vcos/thread.h>

#include "common/locator-getters.hpp"
#include "common/thread.hpp"

using namespace vc;

vcKernelPid vcThreadCreate(vcThread *aThread,
                           char *aStack,
                           int aStackSize,
                           char aPriority,
                           int aFlags,
                           vcThreadHandlerFunc aFunction,
                           void *aArg,
                           const char *aName)
{
    return (static_cast<Thread *>(aThread))->Create(aStack,
                                                    aStackSize,
                                                    aPriority,
                                                    aFlags,
                                                    aFunction,
                                                    aArg,
                                                    aName);
}


int vcThreadGetContexSwitchRequest(void)
{
    return static_cast<int>(Instance::Get().Get<ThreadScheduler>().GetContextSwitchRequest());
}

void vcThreadTaskExit(void)
{
    Instance::Get().Get<ThreadScheduler>().TaskExit();
}

void vcSchedRun(void)
{
    Instance::Get().Get<ThreadScheduler>().Run();
}
