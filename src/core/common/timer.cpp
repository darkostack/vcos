#include "common/timer.hpp"
#include "common/locator-getters.hpp"

#define ENABLE_DEBUG (0)
#include "common/debug.h"

namespace vc {

void Timer::Init(vcTimerCallback aCallback, void *aArg)
{
    this->mNext       = NULL;
    this->mTarget     = 0;
    this->mLongTarget = 0;

    this->mCallback = aCallback;
    this->mArg      = aArg;
}

void Timer::Set(uint32_t aOffset)
{
    DEBUG("Timer::Set() offset=%lu now=%lu\r\n", aOffset, Get<TimerScheduler>().Now());

    if (!this->mCallback)
    {
        DEBUG("TimerScheduler::Set() timer has no callback.\r\n");
        return;
    }

    Get<TimerScheduler>().Remove(this);

    if (aOffset < Get<TimerScheduler>().GetTimerBackoff())
    {
        Get<TimerScheduler>().Spin(aOffset);
        this->Shoot();
    }
    else
    {
        uint32_t target = Get<TimerScheduler>().Now() + aOffset;
        Get<TimerScheduler>().SetAbsolute(this, target);
    }
}

void Timer::Set64(uint32_t aOffset, uint32_t aLongOffset)
{
    Get<TimerScheduler>().Set64(this, aOffset, aLongOffset);
}

void Timer::Remove(void)
{
    unsigned state = vcIrqDisable();

    if (this->IsSet())
    {
        Get<TimerScheduler>().Remove(this);
    }

    vcIrqRestore(state);
}

extern "C" void vcPeriphTimerCallback(void *aArg, int aChannel)
{
    (void)aChannel;
    Instance *instance = static_cast<Instance *>(aArg);
    instance->Get<TimerScheduler>().Callback();
}

TimerScheduler::TimerScheduler(Instance &aInstance)
    : mInHandler(0)
    , mLongCnt(0)
#if VCOS_CONFIG_TIMER_MASK
    , mHighCnt(0)
#endif
    , mTimerListHead(NULL)
    , mOverflowListHead(NULL)
    , mLongListHead(NULL)
{
    /* Initialize low-level timer */
    vcTimInit(VCOS_CONFIG_TIMER_DEV, kTimerHz, vcPeriphTimerCallback, static_cast<void *>(&aInstance));

    /* register initial overflow tick */
    LowLevelTimerSet(0xffffffff);
}

void TimerScheduler::SetAbsolute(Timer *aTimer, uint32_t aTarget)
{
    uint32_t now = Now();

    aTimer->mNext = NULL;

    /* Ensure that offset is bigger than 'kTimerBackoff',
     * 'target - now' will allways be the offset no matter if target < or > now.
     *
     * This expects that target was not set too close to now and overrun now, so
     * from setting target up until the call of 'Now()' above now has not
     * become equal or bigger than target.
     * This is crucial when using low CPU frequencies so reaching the 'Now()'
     * call needs multiple xtimer ticks.
     *
     * 'Set()' and `PeriodicWakeup()` ensure this by already
     * backing off for small values. */
    uint32_t offset = (aTarget - now);

    DEBUG("TimerScheduler::Set() now=%lu, target=%lu, offset=%lu\r\n", now, aTarget, offset);

    if (offset <= kTimerBackoff)
    {
        /* backoff */
        SpinUntil(aTarget);
        aTimer->Shoot();
        return;
    }

    unsigned state = vcIrqDisable();

    if (aTimer->IsSet())
    {
        Remove(aTimer);
    }

    aTimer->mTarget     = aTarget;
    aTimer->mLongTarget = mLongCnt;

    /* Ensure timer is fired in right timer period.
     * Backoff condition above ensures that 'target - kTimerOverhead` is later
     * than 'now', also for values when now will overflow and the value of target
     * is smaller then now.
     * If `target < XTIMER_OVERHEAD` the new target will be at the end of this
     * 32bit period, as `target - XTIMER_OVERHEAD` is a big number instead of a
     * small at the beginning of the next period. */
    aTarget = aTarget - kTimerOverhead;

    /* 32 bit target overflow, target is in next 32bit period */
    if (aTarget < now)
    {
        aTimer->mLongTarget++;
    }

    if ((aTimer->mLongTarget > mLongCnt) || !ThisHighPeriod(aTarget))
    {
        DEBUG("TimerScheduler::Set() the timer doesn't fit into low-level timer's mask.\r\n");
        AddTimerToLongList(&mLongListHead, aTimer);
    }
    else
    {
        if (LowLevelTimerMask(now) >= aTarget)
        {
            DEBUG("TimerScheduler::Set() the timer will expire in the next timer period.\r\n");
            AddTimerToList(&mOverflowListHead, aTimer);
        }
        else
        {
            DEBUG("TimerScheduler::Set() timer will expire in this timer period.\r\n");
            AddTimerToList(&mTimerListHead, aTimer);

            if (mTimerListHead == aTimer)
            {
                DEBUG("TimerScheduler::Set() timer is new list head. updating low level timer\r\n");
                LowLevelTimerSet(aTarget);
            }
        }
    }

    vcIrqRestore(state);
}

void TimerScheduler::Set64(Timer *aTimer, uint32_t aOffset, uint32_t aLongOffset)
{
    DEBUG("TimerScheduler::Set64() offset=%lu, longOffset=%lu\r\n", aOffset, aLongOffset);

    if (!aLongOffset)
    {
        /* timer fits into the short timer */
        aTimer->Set(aOffset);
    }
    else
    {
        unsigned state = vcIrqDisable();

        if (aTimer->IsSet())
        {
            Remove(aTimer);
        }

        NowInternal(&aTimer->mTarget, &aTimer->mLongTarget);

        aTimer->mTarget += aOffset;
        aTimer->mLongTarget += aLongOffset;

        if (aTimer->mTarget < aOffset)
        {
            aTimer->mLongTarget++;
        }

        AddTimerToLongList(&mLongListHead, aTimer);

        vcIrqRestore(state);

        DEBUG("TimerScheduler::Set64() added long term timer (longTarget=%lu, target=%lu)\r\n", aTimer->mLongTarget,
              aTimer->mTarget);
    }
}

void TimerScheduler::Remove(Timer *aTimer)
{
    if (mTimerListHead == aTimer)
    {
        uint32_t next;

        mTimerListHead = static_cast<Timer *>(aTimer->mNext);

        if (mTimerListHead)
        {
            /* schedule callback on next timer target timer */
            next = mTimerListHead->mTarget - kTimerOverhead;
        }
        else
        {
            next = LowLevelTimerMask(0xffffffff);
        }

        LowLevelTimerSet(next);
    }
    else
    {
        if (!RemoveTimerFromList(&mTimerListHead, aTimer))
        {
            if (!RemoveTimerFromList(&mOverflowListHead, aTimer))
            {
                RemoveTimerFromList(&mLongListHead, aTimer);
            }
        }
    }
}

void TimerScheduler::Callback(void)
{
    uint32_t nextTarget;
    uint32_t reference;

    mInHandler = 1;

    DEBUG("TimerScheduler::Callback() now=%lu (%lu)pleft=%lu\r\n", Now(), LowLevelTimerMask(Now()),
          LowLevelTimerMask(0xffffffff - Now()));

    if (!mTimerListHead)
    {
        DEBUG("TimerScheduler::Callback() tick\r\n");

        /* there's no timer for this timer period,
         * so this was a timer overflow callback.
         *
         * In this case, we advance to the next timer period.
         */

        NextPeriod();

        reference = 0;

        /* make sure the timer counter also arrived
         * in the next timer period */
        while (LowLevelTimerNow() == LowLevelTimerMask(0xffffffff))
        {
        }
    }
    else
    {
        /* we ended up in _timer_callback and there is
         * a timer waiting.
         */

        /* set our period reference to the current time. */

        reference = LowLevelTimerNow();
    }

overflow:
    /* check if next timers are close to expiring */
    while (mTimerListHead && (TimeLeft(LowLevelTimerMask(mTimerListHead->mTarget), reference) < kTimerIsrBackoff))
    {
        /* make sure we don't fire too early */
        while (TimeLeft(LowLevelTimerMask(mTimerListHead->mTarget), reference))
        {
        }

        /* pick first timer in list */
        Timer *timer = mTimerListHead;

        /* advance list */
        mTimerListHead = static_cast<Timer *>(timer->mNext);

        /* make sure timer is recognized as being already fired */
        timer->mTarget     = 0;
        timer->mLongTarget = 0;

        /* fire timer */
        timer->Shoot();
    }

    /* possibly executing all callbacks took enough
     * time to overflow.  In that case we advance to
     * next timer period and check again for expired
     * timers.*/

    /* check if the end of this period is very soon */
    uint32_t now = LowLevelTimerNow() + kTimerIsrBackoff;

    if (now < reference)
    {
        DEBUG("TimerScheduler::Callback() overflowed while executing callbacks.%i\r\n", mTimerListHead != NULL);

        NextPeriod();

        /* wait till overflow */
        while (reference < LowLevelTimerNow())
        {
        }
        reference = 0;
        goto overflow;
    }

    if (mTimerListHead)
    {
        /* schedule callback on next timer target time */
        nextTarget = mTimerListHead->mTarget - kTimerOverhead;

        /* make sure we're not setting a time in the past */
        if (nextTarget < (Now() + kTimerIsrBackoff))
        {
            goto overflow;
        }
    }
    else
    {
        /* there's no timer planned for this timer period */
        /* schedule callback on next overflow */

        nextTarget = LowLevelTimerMask(0xffffffff);

        now = LowLevelTimerNow();

        /* check for overflow again */
        if (now < reference)
        {
            NextPeriod();
            reference = 0;
            goto overflow;
        }
        else
        {
            /* check if the end of this period is very soon */
            if (LowLevelTimerMask(now + kTimerIsrBackoff) < now)
            {
                /* spin until next period, then advance */
                while (LowLevelTimerNow() >= now)
                {
                }
                NextPeriod();
                reference = 0;
                goto overflow;
            }
        }
    }

    mInHandler = 0;

    /* set low level timer */
    LowLevelTimerSet(nextTarget);
}

uint32_t TimerScheduler::Now(void)
{
#if VCOS_CONFIG_TIMER_MASK
    uint32_t latchedHighCnt;
    uint32_t now;

    do
    {
        latchedHighCnt = mHighCnt;
        now            = LowLevelTimerNow();
    } while (mHighCnt != latchedHighCnt);

    return latchedHighCnt | now;
#else
    return LowLevelTimerNow();
#endif
}

uint64_t TimerScheduler::Now64(void)
{
    uint32_t shortTerm, longTerm;

    NowInternal(&shortTerm, &longTerm);

    return ((uint64_t)longTerm << 32) + shortTerm;
}

void TimerScheduler::Spin(uint32_t aOffset)
{
    uint32_t start = LowLevelTimerNow();
#if VCOS_CONFIG_TIMER_MASK
    aOffset = LowLevelTimerMask(aOffset);
    while (LowLevelTimerMask(LowLevelTimerNow() - start) < aOffset)
        ;
#else
    while ((LowLevelTimerNow() - start) < aOffset)
        ;
#endif
}

void TimerScheduler::NowInternal(uint32_t *aShortTerm, uint32_t *aLongTerm)
{
    uint32_t before, after, longValue;

    /* loop to cope with possible overflow of Now() */
    do
    {
        before    = Now();
        longValue = mLongCnt;
        after     = Now();
    } while (before > after);

    *aShortTerm = after;
    *aLongTerm  = longValue;
}

extern "C" void sleep64UnlockMutexCallback(void *aArg)
{
    Mutex *mutex = static_cast<Mutex *>(aArg);
    mutex->Unlock();
}

void TimerScheduler::Sleep64(uint32_t aOffset, uint32_t aLongOffset)
{
    if (vcIrqIsIn())
    {
        assert(!aLongOffset);
        Spin(aOffset);
        return;
    }

    Timer timer;
    Mutex mutex;

    timer.mCallback = sleep64UnlockMutexCallback;
    timer.mArg      = static_cast<void *>(&mutex);

    timer.mTarget     = 0;
    timer.mLongTarget = 0;

    mutex.Lock();
    timer.Set64(aOffset, aLongOffset);
    mutex.Lock();
}

void TimerScheduler::AddTimerToList(Timer **aListHead, Timer *aTimer)
{
    while (*aListHead && (*aListHead)->mTarget <= aTimer->mTarget)
    {
        aListHead = reinterpret_cast<Timer **>(&((*aListHead)->mNext));
    }

    aTimer->mNext = *aListHead;
    *aListHead    = aTimer;
}

void TimerScheduler::AddTimerToLongList(Timer **aListHead, Timer *aTimer)
{
    while (*aListHead &&
           (((*aListHead)->mLongTarget < aTimer->mLongTarget) ||
            (((*aListHead)->mLongTarget == aTimer->mLongTarget) && ((*aListHead)->mTarget <= aTimer->mTarget))))
    {
        aListHead = reinterpret_cast<Timer **>(&((*aListHead)->mNext));
    }

    aTimer->mNext = *aListHead;
    *aListHead    = aTimer;
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

uint32_t TimerScheduler::LowLevelTimerNow(void)
{
    return vcTimRead(VCOS_CONFIG_TIMER_DEV);
}

uint32_t TimerScheduler::LowLevelTimerMask(uint32_t aVal)
{
    return aVal & ~kTimerMask;
}

void TimerScheduler::LowLevelTimerSet(uint32_t aTarget)
{
    if (mInHandler)
    {
        return;
    }
    vcTimSetAbsolute(VCOS_CONFIG_TIMER_DEV, VCOS_CONFIG_TIMER_CHAN, LowLevelTimerMask(aTarget));
}

int TimerScheduler::ThisHighPeriod(uint32_t aTarget)
{
#if VCOS_CONFIG_TIMER_MASK
    return (aTarget & kTimerMask) == mHighCnt;
#else
    (void)aTarget;
    return 1;
#endif
}

void TimerScheduler::SpinUntil(uint32_t aTarget)
{
#if VCOS_CONFIG_TIMER_MASK
    aTarget = LowLevelTimerMask(aTarget);
#endif
    while (LowLevelTimerNow() > aTarget)
    {
    }
    while (LowLevelTimerNow() < aTarget)
    {
    }
}

void TimerScheduler::SelectLongTimers(void)
{
    Timer *selectListStart = mLongListHead;
    Timer *selectListLast  = NULL;

    /* advance long list head so it points to the first timer of the next (not
     * just started) "long timer period" */
    while (mLongListHead)
    {
        if ((mLongListHead->mLongTarget <= mLongCnt) && ThisHighPeriod(mLongListHead->mTarget))
        {
            selectListLast = mLongListHead;
            mLongListHead  = static_cast<Timer *>(mLongListHead->mNext);
        }
        else
        {
            /* remaining longList timers belong to later long periods */
            break;
        }
    }

    /* cut the "selected long timer list" at the end */
    if (selectListLast)
    {
        selectListLast->mNext = NULL;
    }

    /* merge "current timer list" and "selected long timer list" */
    if (mTimerListHead)
    {
        if (selectListLast)
        {
            /* both lists are non-empty. merge. */
            mTimerListHead = MergeLists(mTimerListHead, selectListStart);
        }
        else
        {
            /* "selected long timer list" is empty, nothing to do */
        }
    }
    else /* current timer list is empty */
    {
        if (selectListLast)
        {
            /* there's no current timer list, but a non-empty "selected long
             * timer list".  So just use that list as the new current timer
             * list.*/
            mTimerListHead = selectListStart;
        }
    }
}

void TimerScheduler::NextPeriod(void)
{
#if VCOS_CONFIG_TIMER_MASK
    /* advance < 32bit mask register */
    mHighCnt += ~kTimerMask + 1;

    if (mHighCnt == 0)
    {
        /* mHighCnt overflowed, so advance > 32bit counter */
        mLongCnt++;
    }
#else
    /* advance > 32bit counter */
    mLongCnt++;
#endif

    /* swap overflow list to current timer list */
    mTimerListHead    = mOverflowListHead;
    mOverflowListHead = NULL;

    SelectLongTimers();
}

uint32_t TimerScheduler::TimeLeft(uint32_t aTarget, uint32_t aReference)
{
    uint32_t now = LowLevelTimerNow();

    if (now < aReference)
    {
        return 0;
    }

    if (aTarget > now)
    {
        return aTarget - now;
    }
    else
    {
        return 0;
    }
}

Timer *TimerScheduler::Compare(Timer *aTimerA, Timer *aTimerB)
{
    if (aTimerA && aTimerB)
    {
        return ((aTimerA->mTarget <= aTimerB->mTarget) ? aTimerA : aTimerB);
    }
    else
    {
        return (aTimerA ? aTimerA : aTimerB);
    }
}

Timer *TimerScheduler::MergeLists(Timer *aHeadA, Timer *aHeadB)
{
    Timer *resultHead = Compare(aHeadA, aHeadB);
    Timer *pos        = resultHead;

    while (1)
    {
        aHeadA = static_cast<Timer *>(aHeadA->mNext);
        aHeadB = static_cast<Timer *>(aHeadB->mNext);

        if (!aHeadA)
        {
            pos->mNext = aHeadB;
            break;
        }

        if (!aHeadB)
        {
            pos->mNext = aHeadA;
            break;
        }

        pos->mNext = Compare(aHeadA, aHeadB);
        pos        = static_cast<Timer *>(pos->mNext);
    }

    return resultHead;
}

} // namespace vc
