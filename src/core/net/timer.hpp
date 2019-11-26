#ifndef NET_TIMER_HPP
#define NET_TIMER_HPP

#include "vcos-core-config.h"

#include <stddef.h>
#include <stdint.h>

#include <vcos/net/platform/alarm-micro.h>
#include <vcos/net/platform/alarm-milli.h>

#include "common/locator.hpp"

#include "net/time.hpp"

namespace vc {
namespace Net {

class TimerMilliScheduler;

class Timer : public InstanceLocator
{
    friend class TimerScheduler;

public:
    static const uint32_t kMaxDelay = (Time::kMaxDuration >> 1);

    typedef void (*Handler)(Timer &aTimer);

    Timer(Handler aHandler)
        : mHandler(aHandler)
        , mFireTime()
        , mNext(this)
    {
    }

    Time GetFireTime(void) const { return mFireTime; }

    bool IsRunning(void) const { return (mNext != this); }

protected:
    bool DoesFireBefore(const Timer &aSecondTimer, Time aNow);

    void Fired(void) { mHandler(*this); }

    Handler mHandler;
    Time mFireTime;
    Timer *mNext;
};

class TimerMilli : public Timer
{
public:
    TimerMilli(Handler aHandler)
        : Timer(aHandler)
    {
    }

    void Start(uint32_t aDelay);

    void StartAt(TimeMilli sStartTime, uint32_t aDelay);

    void Stop(void);

    static TimeMilli GetNow(void) { return TimeMilli(vcNetPlatAlarmMilliGetNow()); }
};

class TimerMilliContext : public TimerMilli
{
public:
    TimerMilliContext(Handler aHandler, void *aContext)
        : TimerMilli(aHandler)
        , mContext(aContext)
    {
    }

    void *GetContext(void) { return mContext; }

private:
    void *mContext;
};

class TimerScheduler : public InstanceLocator
{
    friend class Timer;

protected:
    struct AlarmApi
    {
        void (*AlarmStartAt)(vcInstance *aInstance, uint32_t aT0, uint32_t aDt);
        void (*AlarmStop)(vcInstance *aInstance);
        uint32_t (*AlarmGetNow)(void);
    };

    explicit TimerScheduler(void)
        : mHead(NULL)
    {
    }

    void Add(Timer &aTimer, const AlarmApi &aAlarmApi);

    void Remove(Timer &aTimer, const AlarmApi &aAlarmApi);

    void ProcessTimers(const AlarmApi &aAlarmApi);

    void SetAlarm(const AlarmApi &aAlarmApi);

    Timer *mHead;
};

class TimerMilliScheduler : public TimerScheduler
{
public:
    explicit TimerMilliScheduler(void)
        : TimerScheduler()
    {
    }

    void Add(TimerMilli &aTimer) { TimerScheduler::Add(aTimer, sAlarmMilliApi); }

    void Remove(TimerMilli &aTimer) { TimerScheduler::Remove(aTimer, sAlarmMilliApi); }

    void ProcessTimers(void) { TimerScheduler::ProcessTimers(sAlarmMilliApi); }

private:
    static const AlarmApi sAlarmMilliApi;
};

class TimerMicroScheduler;

class TimerMicro : public Timer
{
public:
    TimerMicro(Handler aHandler)
        : Timer(aHandler)
    {
    }

    void Start(uint32_t aDelay);

    void StartAt(TimeMicro aStartTime, uint32_t aDelay);

    void Stop(void);

    static TimeMicro GetNow(void) { return Time(vcNetPlatAlarmMicroGetNow()); }
};

class TimerMicroScheduler : public TimerScheduler
{
public:
    explicit TimerMicroScheduler(void)
        : TimerScheduler()
    {
    }

    void Add(TimerMicro &aTimer) { TimerScheduler::Add(aTimer, sAlarmMicroApi); }

    void Remove(TimerMicro &aTimer) { TimerScheduler::Remove(aTimer, sAlarmMicroApi); }

    void ProcessTimers(void) { TimerScheduler::ProcessTimers(sAlarmMicroApi); }

private:
    static const AlarmApi sAlarmMicroApi;
};

} // namespace Net
} // namespace vc

#endif /* NET_TIMER_HPP */
