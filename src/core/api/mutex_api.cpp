#include <vcos/mutex.h>

#include "common/mutex.hpp"

using namespace vc;

void vcMutexLock(vcMutex *aMutex)
{
    Mutex &mutex = *static_cast<Mutex *>(aMutex);
    mutex.Lock();
}

void vcMutexUnlock(vcMutex *aMutex)
{
    Mutex &mutex = *static_cast<Mutex *>(aMutex);
    mutex.Unlock();
}
