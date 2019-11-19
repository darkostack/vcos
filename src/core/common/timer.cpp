#include "common/timer.hpp"


namespace vc {

void TimerScheduler::AddTimerToList(Timer **aListHead, Timer *aTimer)
{
    while (*aListHead && (*aListHead)->mTarget <= aTimer->mTarget)
    {
        aListHead = reinterpret_cast<Timer **>(&((*aListHead)->mNext));
    }

    aTimer->mNext = *aListHead;
    *aListHead = aTimer;
}

void TimerScheduler::AddTimerToLongList(Timer **aListHead, Timer *aTimer)
{
    while (*aListHead && (((*aListHead)->mLongTarget < aTimer->mLongTarget) ||
                          (((*aListHead)->mLongTarget == aTimer->mLongTarget) &&
                           ((*aListHead)->mTarget <= aTimer->mTarget))))
    {
        aListHead = reinterpret_cast<Timer **>(&((*aListHead)->mNext));
    }

    aTimer->mNext = *aListHead;
    *aListHead = aTimer;
}

int TimerScheduler::RemoveTimerFromList(Timer **aListHead, Timer *aTimer)
{
    while (*aListHead)
    {
        if (*aListHead == aTimer)
        {
            *aListHead = static_cast<Timer *>(aTimer->mNext);
            return 1;
        }
        aListHead = reinterpret_cast<Timer **>(&((*aListHead)->mNext));
    }

    return 0;
}

void TimerScheduler::Remove(Timer *aTimer)
{
    if (mTimerListHead == aTimer)
    {
        mTimerListHead = static_cast<Timer *>(aTimer->mNext);

        if (mTimerListHead)
        {
            /* TODO: schedule callback on next timer target timer */
        }
        else
        {
            /* TODO */
        }
    }
    else
    {
        if (!RemoveTimerFromList(&mTimerListHead, aTimer)) {
            if (!RemoveTimerFromList(&mOverflowListHead, aTimer)) {
                RemoveTimerFromList(&mLongListHead, aTimer);
            }
        }
    }
}

} // namespace vc
