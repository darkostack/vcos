#include "common/mutex.hpp"

namespace vc {

int Mutex::SetLock(int aBlocking)
{
    return aBlocking;
}

void Mutex::Unlock(void)
{
    return;
}

void Mutex::UnlockAndSleep(void)
{
    return;
}

} // namespace vc
