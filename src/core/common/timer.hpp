#ifndef TIMER_HPP
#define TIMER_HPP

#include <vcos/irq.h>
#include <vcos/periph/tim.h>
#include <vcos/timer.h>
#include <vcos/types.h>

#include "common/locator.hpp"
#include "common/mutex.hpp"

#include "vcos-core-config.h"

namespace vc {

class TimerScheduler;

class Timer : public vcTimer, public InstanceLocator
{
    friend class TimerScheduler;

public:
    void Init(vcTimerCallback aCallback, void *aArg);

    void Set(uint32_t aOffset);

    void Set64(uint32_t aOffset, uint32_t aLongOffset);

    int IsSet(void) { return (mTarget || mLongTarget); }

    void Shoot(void) { this->mCallback(this->mArg); }

    void Remove(void);
};

class TimerScheduler
{
public:
    TimerScheduler(Instance &aInstance);

    void SetAbsolute(Timer *aTimer, uint32_t aTarget);

    void Set64(Timer *aTimer, uint32_t aOffset, uint32_t aLongOffset);

    void Remove(Timer *aTimer);

    void Callback(void);

    uint32_t Now(void);

    uint64_t Now64(void);

    void Spin(uint32_t aOffset);

    uint32_t GetTimerBackoff(void) { return kTimerBackoff; }

    void NowInternal(uint32_t *aShortTerm, uint32_t *aLongTerm);

    void Sleep64(uint32_t aOffset, uint32_t aLongOffset);

private:
    static void AddTimerToList(Timer **aListHead, Timer *aTimer);

    static void AddTimerToLongList(Timer **aListHead, Timer *aTimer);

    static int RemoveTimerFromList(Timer **aListHead, Timer *aTimer);

    static uint32_t LowLevelTimerNow(void);

    static uint32_t LowLevelTimerMask(uint32_t aVal);

    void LowLevelTimerSet(uint32_t aTarget);

    int ThisHighPeriod(uint32_t aTarget);

    void SpinUntil(uint32_t aTarget);

    void SelectLongTimers(void);

    void NextPeriod(void);

    uint32_t TimeLeft(uint32_t aTarget, uint32_t aReference);

    Timer *Compare(Timer *aTimerA, Timer *aTimerB);

    Timer *MergeLists(Timer *aHeadA, Timer *aHeadB);

    enum
    {
        kTimerBackoff          = VCOS_CONFIG_TIMER_BACKOFF,
        kTimerOverhead         = VCOS_CONFIG_TIMER_OVERHEAD,
        kTimerIsrBackoff       = VCOS_CONFIG_TIMER_ISR_BACKOFF,
        kTimerPeriodicSpin     = (kTimerBackoff * 2),
        kTimerPeriodicRelative = VCOS_CONFIG_TIMER_PERIODIC_RELATIVE,
        kTimerMask             = VCOS_CONFIG_TIMER_MASK,
        kTimerHz               = VCOS_CONFIG_TIMER_HZ,
        kTimerShift            = VCOS_CONFIG_TIMER_SHIFT,
    };

    int      mInHandler;
    uint32_t mLongCnt;
#if VCOS_CONFIG_TIMER_MASK
    uint32_t mHighCnt;
#endif
    Timer *mTimerListHead;
    Timer *mOverflowListHead;
    Timer *mLongListHead;
};

} // namespace vc

#endif /* TIMER_HPP */
