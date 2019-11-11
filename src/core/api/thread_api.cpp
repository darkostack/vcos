#include <vcos/thread.h>

#include "common/locator-getters.hpp"
#include "common/thread.hpp"

using namespace vc;

void vcThreadTaskExit(void)
{
    Instance::Get().Get<ThreadScheduler>().TaskExit();
}

void *vcSchedGetActiveThread(void)
{
    return static_cast<void *>(Instance::Get().Get<ThreadScheduler>().GetSchedActiveThread());
}

void vcSchedRun(void)
{
    Instance::Get().Get<ThreadScheduler>().Run();
}
