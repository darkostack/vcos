#include <vcos/irq.h>

#include "common/mutex.hpp"
#include "common/thread.hpp"

#define ENABLE_DEBUG (0)
#include "common/debug.h"

#if ENABLE_DEBUG
#include <inttypes.h>
#endif

namespace vc {

int Mutex::SetLock(int aBlocking)
{
    unsigned state = irqDisable();

    DEBUG("Mutex::SetLock() PID(%" PRIKernel_pid "): mutex in use.\n")
}

} // namespace vc
