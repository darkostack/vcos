#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <stddef.h>
#include <stdint.h>

#include "common/list.hpp"
#include "common/locator.hpp"
#include "common/thread.hpp"

#define MUTEX_LOCKED ((ListNode *)-1)

namespace vc {

class Mutex : public InstanceLocator, public List
{
public:
    Mutex(Instance &aInstance)
        : InstanceLocator(aInstance)
    {
        mQueue.mNext = NULL;
    }

    int TryLock(void) { return SetLock(0); }

    void Lock(void) { SetLock(1); }

    void Unlock(void);

    void UnlockAndSleep(void);

private:
    int SetLock(int aBlocking);

    ListNode mQueue;
};

} // namespace vc

#endif /* MUTEX_HPP */
