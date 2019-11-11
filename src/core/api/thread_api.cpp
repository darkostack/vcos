#include <vcos/thread.h>

#include "common/locator-getters.hpp"
#include "common/thread.hpp"

namespace vc {

extern "C" void vcThreadTaskExit(void)
{
    Instance::Get().Get<ThreadScheduler>().TaskExit();
}

extern "C" void vcSchedRun(void)
{
    Instance::Get().Get<ThreadScheduler>().Run();
}

} // namespace vc
