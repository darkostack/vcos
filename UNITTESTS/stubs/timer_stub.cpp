#include "common/timer.hpp"
#include "common/locator-getters.hpp"

namespace vc {

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
}

} // namespace vc
