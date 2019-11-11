#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <stddef.h>
#include <stdint.h>

#include "common/list.hpp"

namespace vc {

class Mutex
{
    Mutex(void)
    {
        mQueue.mNext = NULL;
    }

    int TryLock(void) { return SetLock(0); }

    void Lock(void) { return SetLock(1); }

    void Unlock(void);

private:
    int SetLock(int aBlocking);

    ListNode mQueue;
};

} // namespace vc

#endif /* MUTEX_HPP */
