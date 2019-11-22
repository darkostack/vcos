#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <stddef.h>
#include <stdint.h>

#include <vcos/mutex.h>

#include "common/list.hpp"
#include "common/locator.hpp"

namespace vc {

class Mutex : public vcMutex, public InstanceLocator
{
public:
    explicit Mutex(void) { mQueue.mNext = NULL; }

    int TryLock(void) { return SetLock(0); }

    void Lock(void) { SetLock(1); }

    void Unlock(void);

    void UnlockAndSleep(void);

private:
    int SetLock(int aBlocking);
};

} // namespace vc

#endif /* MUTEX_HPP */
