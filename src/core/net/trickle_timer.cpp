#include "net/trickle_timer.hpp"

#include "common/code_utils.hpp"
#include "common/random.hpp"

namespace vc {
namespace Net {

TrickleTimer::TrickleTimer(uint32_t aRedundancyConstant, Handler aTransmitHandler, Handler aIntervalExpiredHandler)
    : TimerMilli(&TrickleTimer::HandleTimer)
    , mRedundancyConstant(aRedundancyConstant)
    , mCounter(0)
    , mIntervalMin(0)
    , mIntervalMax(0)
    , mInterval(0)
    , mTimeInInterval(0)
    , mTransmitHandler(aTransmitHandler)
    , mIntervalExpiredHandler(aIntervalExpiredHandler)
    , mMode(kModeNormal)
    , mIsRunning(false)
    , mInTransmitPhase(false)
{
    assert(aTransmitHandler != NULL);
}

vcNetError TrickleTimer::Start(uint32_t aIntervalMin, uint32_t aIntervalMax, Mode aMode)
{
    vcNetError error = VC_NET_ERROR_NONE;

    VerifyOrExit(aIntervalMax >= aIntervalMin, error = VC_NET_ERROR_INVALID_ARGS);
    VerifyOrExit(aIntervalMin != 0 || aIntervalMax != 0, error = VC_NET_ERROR_INVALID_ARGS);

    mIntervalMin = aIntervalMin;
    mIntervalMax = aIntervalMax;
    mMode        = aMode;
    mIsRunning   = true;

    // Select interval randomly from range [Imin, Imax].
    mInterval = Random::GetUint32InRange(mIntervalMin, mIntervalMax + 1);

    StartNewInterval();

exit:
    return error;
}

void TrickleTimer::Stop(void)
{
    mIsRunning = false;
    TimerMilli::Stop();
}

void TrickleTimer::IndicateInconsistent(void)
{
    // If interval is equal to minimum when an "inconsistent" event
    // is received, do nothing.
    VerifyOrExit(mIsRunning && (mInterval != mIntervalMin));

    mInterval = mIntervalMin;
    StartNewInterval();

exit:
    return;
}

void TrickleTimer::StartNewInterval(void)
{
    uint32_t halfInterval;

    mCounter = 0;

    mInTransmitPhase = true;

    switch (mMode)
    {
    case kModeNormal:
        halfInterval = mInterval / 2;
        VerifyOrExit(halfInterval < mInterval, mTimeInInterval = halfInterval);

        // Select a random point in the interval taken from the range [I/2, I).
        mTimeInInterval = Random::GetUint32InRange(halfInterval, mInterval);
        break;

    case kModePlainTimer:
        mTimeInInterval = mInterval;
        break;

    case kModeMPL:
        // Select a random point in interval taken from the range [0, I].
        mTimeInInterval = Random::GetUint32InRange(0, mInterval + 1);
        break;
    }

exit:
    TimerMilli::Start(mTimeInInterval);
}

void TrickleTimer::HandleTimer(Timer &aTimer)
{
    static_cast<TrickleTimer *>(&aTimer)->HandleTimer();
}

void TrickleTimer::HandleTimer(void)
{
    if (mInTransmitPhase)
    {
        HandleEndOfTimeInInterval();
    }
    else
    {
        HandleEndOfInterval();
    }
}

void TrickleTimer::HandleEndOfTimeInInterval(void)
{
    // Trickle transmits if and only if the counter `c` is less
    // than the redundancy constant `k`.
    if (mRedundancyConstant == 0 || mCounter < mRedundancyConstant)
    {
        bool shouldContinue = mTransmitHandler(*this);
        VerifyOrExit(shouldContinue, Stop());
    }

    switch (mMode)
    {
    case kModePlainTimer:
        // Select a random interval in [Imin, Imax] and restart.
        mInterval = Random::GetUint32InRange(mIntervalMin, mIntervalMax + 1);
        StartNewInterval();
        break;

    case kModeNormal:
    case kModeMPL:
        // Waiting for the rest of the interval to elapse.
        mInTransmitPhase = false;
        TimerMilli::Start(mInterval - mTimeInInterval);
        break;
    }

exit:
    return;
}

void TrickleTimer::HandleEndOfInterval(void)
{
    // Double the interval and ensure result is below max.
    if (mInterval == 0)
    {
        mInterval = 1;
    }
    else if (mInterval <= mIntervalMax - mInterval)
    {
        mInterval *= 2;
    }
    else
    {
        mInterval = mIntervalMax;
    }

    if (mIntervalExpiredHandler)
    {
        bool shouldContinue = mIntervalExpiredHandler(*this);
        VerifyOrExit(shouldContinue, Stop());
    }

    StartNewInterval();

exit:
    return;
}

} // namespace Net
} // namespace vc
