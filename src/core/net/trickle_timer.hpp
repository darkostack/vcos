#ifndef NET_TRICKLE_TIMER_HPP
#define NET_TRICKLE_TIMER_HPP

#include "vcos-core-config.h"

#include <vcos/net/error.h>

#include "net/timer.hpp"

namespace vc {
namespace Net {

class TrickleTimer : public TimerMilli
{
public:
    enum Mode
    {
        kModeNormal,
        kModePlainTimer,
        kModeMPL,
    };

    typedef bool (*Handler)(TrickleTimer &aTimer);

    TrickleTimer(uint32_t aRedundancyConstant, Handler aTransmitHandler, Handler aIntervalExpiredHandler);

    bool IsRunning(void) const { return mIsRunning; }

    vcNetError Start(uint32_t aIntervalMin, uint32_t aIntervalMax, Mode aMode);

    void Stop(void);

    void IndicateConsistent(void) { mCounter++; }

    void IndicateInconsistent(void);

private:
    void        StartNewInterval(void);
    static void HandleTimer(Timer &aTimer);
    void        HandleTimer(void);
    void        HandleEndOfTimeInInterval(void);
    void        HandleEndOfInterval(void);
    void        StartAt(void) {}

    const uint32_t mRedundancyConstant;
    uint32_t       mCounter;

    uint32_t mIntervalMin;
    uint32_t mIntervalMax;
    uint32_t mInterval;
    uint32_t mTimeInInterval;
    Handler  mTransmitHandler;
    Handler  mIntervalExpiredHandler;
    Mode     mMode;
    bool     mIsRunning : 1;
    bool     mInTransmitPhase : 1;
};

} // namespace Net
} // namespace vc

#endif /* NET_TRICKLE_TIMER_HPP */
