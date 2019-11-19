#include <assert.h>

#include <vcos/timer.h>

#include "common/locator-getters.hpp"
#include "common/timer.hpp"

using namespace vc;

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
