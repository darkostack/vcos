#include <assert.h>

#include <vcos/timer.h>

#include "common/locator-getters.hpp"
#include "common/timer.hpp"

using namespace vc;

void vcTimerInit(vcTimer *aTimer, vcTimerCallback aCallback, void *aArg)
{
    Timer &timer = *static_cast<Timer *>(aTimer);
    timer.Init(aCallback, aArg);
}

vcTimerTicks32 vcTimerNow(void)
{
    vcTimerTicks32 ret;
    ret.mTicks32 = Instance::Get().Get<TimerScheduler>().Now();
    return ret;
}

vcTimerTicsk64 vcTimerNow64(void)
{
    vcTimerTicsk64 ret;
    ret.mTicks64 = Instance::Get().Get<TimerScheduler>().Now64();
    return ret;
}

uint32_t vcTimerNowUsec(void)
{
    assert(VCOS_CONFIG_TIMER_HZ == VCOS_CONFIG_TIMER_HZ_BASE);
    return vcTimerNow().mTicks32;
}

uint64_t vcTimerNowUsec64(void)
{
    assert(VCOS_CONFIG_TIMER_HZ == VCOS_CONFIG_TIMER_HZ_BASE);
    return vcTimerNow64().mTicks64;
}

void vcTimerSet(vcTimer *aTimer, uint32_t aOffset)
{
    Timer &timer = *static_cast<Timer *>(aTimer);
    timer.Set(aOffset);
}

void vcTimerSet64(vcTimer *aTimer, uint32_t aOffset, uint32_t aLongOffset)
{
    Timer &timer = *static_cast<Timer *>(aTimer);
    timer.Set64(aOffset, aLongOffset);
}

void vcTimerSleepUsec(uint32_t aUsec)
{
    Instance::Get().Get<TimerScheduler>().Sleep64(aUsec, 0);
}

void vcTimerSleepUsec64(uint64_t aUsec)
{
    Instance::Get().Get<TimerScheduler>().Sleep64(static_cast<uint32_t>(aUsec),
                                                  static_cast<uint32_t>(aUsec >> 32));
}
