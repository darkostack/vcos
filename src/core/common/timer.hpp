#ifndef TIMER_HPP
#define TIMER_HPP

#include <vcos/timer.h>

#include "common/locator.hpp"

namespace vc {

class TimerScheduler;

class Timer: public vcTimer, public InstanceLocator
{
    friend class TimerScheduler;

};

class TimerScheduler
{
public:
    TimerScheduler(void)
        : mInHandler(0)
        , mLongCnt(0)
        , mTimerListHead(NULL)
        , mOverflowListHead(NULL)
        , mLongListHead(NULL)
    {
    }

    static void AddTimerToList(Timer **aListHead, Timer *aTimer);

    static void AddTimerToLongList(Timer **aListHead, Timer *aTimer);

    static int RemoveTimerFromList(Timer **aListHead, Timer *aTimer);

    void Remove(Timer *aTimer);

private:
    int      mInHandler;
    uint32_t mLongCnt;
    Timer *  mTimerListHead;
    Timer *  mOverflowListHead;
    Timer *  mLongListHead;
};

} // namespace vc

#endif /* TIMER_HPP */
