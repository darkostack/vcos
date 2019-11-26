#include "gtest/gtest.h"

#include "common/instance.hpp"

#include "net/timer.hpp"

#include "test_platform.h"

using namespace vc;

class TestNetTimer : public testing::Test
{
protected:
    Instance *obj;

    virtual void SetUp()
    {
        obj = &Instance::Init();
    }

    virtual void TearDown()
    {
    }
};

TEST_F(TestNetTimer, constructor)
{
    EXPECT_TRUE(obj);
    EXPECT_TRUE(obj->IsInitialized());
}

enum
{
    kCallCountIndexAlarmStop = 0,
    kCallCountIndexAlarmStart,
    kCallCountIndexTimerHandler,

    kCallCountIndexMax
};

uint32_t sNow;
uint32_t sPlatT0;
uint32_t sPlatDt;
bool sTimerOn;
uint32_t sCallCount[kCallCountIndexMax];

void testTimerAlarmStop(vcInstance *)
{
    sTimerOn = false;
    sCallCount[kCallCountIndexAlarmStop]++;
}

void testTimerAlarmStartAt(vcInstance *, uint32_t aT0, uint32_t aDt)
{
    sTimerOn = true;
    sCallCount[kCallCountIndexAlarmStart]++;
    sPlatT0 = aT0;
    sPlatDt = aDt;
}

uint32_t testTimerAlarmGetNow(void)
{
    return sNow;
}

void InitTestTimer(void)
{
    g_testPlatAlarmStop = testTimerAlarmStop;
    g_testPlatAlarmStartAt = testTimerAlarmStartAt;
    g_testPlatAlarmGetNow = testTimerAlarmGetNow;
}

void InitCounters(void)
{
    memset(sCallCount, 0, sizeof(sCallCount));
}

class TestTimer : public Net::TimerMilli
{
public:
    TestTimer(void)
        : Net::TimerMilli(TestTimer::HandleTimerFired)
        , mFiredCounter(0)
    {
    }

    static void HandleTimerFired(Net::Timer &aTimer) { static_cast<TestTimer &>(aTimer).HandleTimerFired(); }

    void HandleTimerFired(void)
    {
        sCallCount[kCallCountIndexTimerHandler]++;
        mFiredCounter++;
    }

    uint32_t GetFiredCounter(void) { return mFiredCounter; }

    void ResetFiredCounter(void) { mFiredCounter = 0; }

private:
    uint32_t mFiredCounter;
};

TEST_F(TestNetTimer, test_one_timer)
{
    const uint32_t kTimeT0 = 1000;
    const uint32_t kTimerInterval = 10;

    TestTimer timer;

    InitTestTimer();
    InitCounters();

    sNow = kTimeT0;

    timer.Start(kTimerInterval);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 0);
    EXPECT_EQ(sPlatT0, 1000);
    EXPECT_EQ(sPlatDt, 10);
    EXPECT_TRUE(timer.IsRunning());
    EXPECT_TRUE(sTimerOn);

    sNow += kTimerInterval;

    vcNetPlatAlarmMilliFired(obj);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 1);
    EXPECT_FALSE(timer.IsRunning());
    EXPECT_FALSE(sTimerOn);

    // Test one Timer that spans the 32-bit wrap.

    InitCounters();

    sNow = 0 - (kTimerInterval - 2);

    timer.Start(kTimerInterval);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 0);
    EXPECT_EQ(sPlatT0, (0 - (kTimerInterval - 2)));
    EXPECT_EQ(sPlatDt, 10);
    EXPECT_TRUE(timer.IsRunning());
    EXPECT_TRUE(sTimerOn);

    sNow += kTimerInterval;

    vcNetPlatAlarmMilliFired(obj);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 1);
    EXPECT_FALSE(timer.IsRunning());
    EXPECT_FALSE(sTimerOn);

    // Test one Timer that is late by several msec

    InitCounters();

    sNow = kTimeT0;

    timer.Start(kTimerInterval);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 0);
    EXPECT_EQ(sPlatT0, 1000);
    EXPECT_EQ(sPlatDt, 10);
    EXPECT_TRUE(timer.IsRunning());
    EXPECT_TRUE(sTimerOn);

    sNow += kTimerInterval + 5;

    vcNetPlatAlarmMilliFired(obj);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 1);
    EXPECT_FALSE(timer.IsRunning());
    EXPECT_FALSE(sTimerOn);

    // Test one Timer that is early by several msec

    InitCounters();

    sNow = kTimeT0;

    timer.Start(kTimerInterval);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 0);
    EXPECT_EQ(sPlatT0, 1000);
    EXPECT_EQ(sPlatDt, 10);
    EXPECT_TRUE(timer.IsRunning());
    EXPECT_TRUE(sTimerOn);

    sNow += kTimerInterval - 2;

    vcNetPlatAlarmMilliFired(obj);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 2);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 0);
    EXPECT_TRUE(timer.IsRunning());
    EXPECT_TRUE(sTimerOn);

    sNow += 2;

    vcNetPlatAlarmMilliFired(obj);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 2);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 1);
    EXPECT_FALSE(timer.IsRunning());
    EXPECT_FALSE(sTimerOn);
}

TEST_F(TestNetTimer, test_two_timer)
{
    const uint32_t kTimeT0 = 1000;
    const uint32_t kTimerInterval = 0;

    TestTimer timer1;
    TestTimer timer2;

    InitTestTimer();
    InitCounters();

    // Test when second timer stars at the fire time of first timer (before alarm callback).

    sNow = kTimeT0;

    timer1.Start(kTimerInterval);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 0);
    EXPECT_EQ(sPlatT0, kTimeT0);
    EXPECT_EQ(sPlatDt, kTimerInterval);
    EXPECT_TRUE(timer1.IsRunning());
    EXPECT_FALSE(timer2.IsRunning());
    EXPECT_TRUE(sTimerOn);

    sNow += kTimerInterval;

    timer2.Start(kTimerInterval);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 0);
    EXPECT_EQ(sPlatT0, kTimeT0);
    EXPECT_EQ(sPlatDt, kTimerInterval);
    EXPECT_TRUE(timer1.IsRunning());
    EXPECT_TRUE(timer2.IsRunning());
    EXPECT_TRUE(sTimerOn);

    vcNetPlatAlarmMilliFired(obj);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 2);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 1);
    EXPECT_EQ(timer1.GetFiredCounter(), 1);
    EXPECT_EQ(sPlatT0, sNow);
    EXPECT_EQ(sPlatDt, kTimerInterval);
    EXPECT_FALSE(timer1.IsRunning());
    EXPECT_TRUE(timer2.IsRunning());
    EXPECT_TRUE(sTimerOn);

    sNow += kTimerInterval;

    vcNetPlatAlarmMilliFired(obj);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 2);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 2);
    EXPECT_EQ(timer2.GetFiredCounter(), 1);
    EXPECT_FALSE(timer1.IsRunning());
    EXPECT_FALSE(timer2.IsRunning());
    EXPECT_FALSE(sTimerOn);

    // Test when second timer starts at the fire time of first timer (before vcNetPlatAlarmMilliFired())
    // and its fire time is before the first timer. Ensure that the second timer handler
    // is invoked before the first one.

    InitCounters();

    timer1.ResetFiredCounter();
    timer2.ResetFiredCounter();

    sNow = kTimeT0;

    timer1.Start(kTimerInterval);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 0);
    EXPECT_EQ(sPlatT0, kTimeT0);
    EXPECT_EQ(sPlatDt, kTimerInterval);
    EXPECT_TRUE(timer1.IsRunning());
    EXPECT_FALSE(timer2.IsRunning());
    EXPECT_TRUE(sTimerOn);

    sNow += kTimerInterval;

    timer2.StartAt(Net::TimeMilli(kTimeT0), kTimerInterval - 2); // Timer 2 is even before timer 1

    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 0);
    EXPECT_TRUE(timer1.IsRunning());
    EXPECT_TRUE(timer2.IsRunning());
    EXPECT_TRUE(sTimerOn);

    vcNetPlatAlarmMilliFired(obj);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 1);
    EXPECT_EQ(timer2.GetFiredCounter(), 1);
    EXPECT_TRUE(timer1.IsRunning());
    EXPECT_FALSE(timer2.IsRunning());
    EXPECT_TRUE(sTimerOn);

    vcNetPlatAlarmMilliFired(obj);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 2);
    EXPECT_EQ(timer1.GetFiredCounter(), 1);
    EXPECT_FALSE(timer1.IsRunning());
    EXPECT_FALSE(timer2.IsRunning());
    EXPECT_FALSE(sTimerOn);

    // Timer 1 fire callback is late by some ticks/ms, and second timer is scheduled (before call to
    // vcNetPlatAlarmMilliFired) with a maximum interval.
    // This is to test (corner-case) scenario where the fire time of two
    // timers spanning over the maximum interval.

    InitCounters();

    timer1.ResetFiredCounter();
    timer2.ResetFiredCounter();

    sNow = kTimeT0;

    timer1.Start(kTimerInterval);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 0);
    EXPECT_EQ(sPlatT0, kTimeT0);
    EXPECT_EQ(sPlatDt, kTimerInterval);
    EXPECT_TRUE(timer1.IsRunning());
    EXPECT_FALSE(timer2.IsRunning());
    EXPECT_TRUE(sTimerOn);

    sNow += kTimerInterval + 5;

    timer2.Start(Net::Timer::kMaxDelay);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 0);
    EXPECT_TRUE(timer1.IsRunning());
    EXPECT_TRUE(timer2.IsRunning());
    EXPECT_TRUE(sTimerOn);

    vcNetPlatAlarmMilliFired(obj);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 2);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 1);
    EXPECT_EQ(timer1.GetFiredCounter(), 1);
    EXPECT_FALSE(timer1.IsRunning());
    EXPECT_TRUE(timer2.IsRunning());
    EXPECT_TRUE(sTimerOn);

    sNow += Net::Timer::kMaxDelay;

    vcNetPlatAlarmMilliFired(obj);

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 2);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 1);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 2);
    EXPECT_EQ(timer2.GetFiredCounter(), 1);
    EXPECT_FALSE(timer1.IsRunning());
    EXPECT_FALSE(timer2.IsRunning());
    EXPECT_FALSE(sTimerOn);
}

static void TenTimers(Instance *aInstance, uint32_t aTimeShift, TestTimer *aTimers, uint32_t aNumTimers)
{
    const uint32_t kNumTimers = 10;

    EXPECT_EQ(kNumTimers, aNumTimers);

    const uint32_t kNumTriggers = 7;
    const uint32_t kTimeT0[kNumTimers] = {1000, 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008};
    const uint32_t kTimerInterval[kNumTimers] = {
        20, 100, (Net::Timer::kMaxDelay - kTimeT0[2]), 100000, 1000000, 10, Net::Timer::kMaxDelay, 200, 200, 200};
    // Expected timer fire order
    // timer #     Trigger time
    //   5            1014
    //   0            1020
    //   1            1100
    //   7            1206
    //   8            1207
    //   9            1208
    //   3          101002
    //   4         1001003
    //   2          kMaxDuration
    //   6   kMaxDuration + 1005
    const uint32_t kTriggerTimes[kNumTriggers] = {
        1014, 1020, 1100, 1207, 101004, Net::Timer::kMaxDelay, Net::Timer::kMaxDelay + kTimeT0[6]};
    // Expected timers fired by each kTriggerTimes[] value
    //  Trigger #    Timers Fired
    //    0             5
    //    1             0
    //    2             1
    //    3             7, 8
    //    4             9, 3
    //    5             4, 2
    //    6             6
    const bool kTimerStateAfterTrigger[kNumTriggers][kNumTimers] = {
        {true, true, true, true, true, false, true, true, true, true},         // 5
        {false, true, true, true, true, false, true, true, true, true},        // 0
        {false, false, true, true, true, false, true, true, true, true},       // 1
        {false, false, true, true, true, false, true, false, false, true},     // 7, 8
        {false, false, true, false, true, false, true, false, false, false},   // 9, 3
        {false, false, false, false, false, false, true, false, false, false}, // 4, 2
        {false, false, false, false, false, false, false, false, false, false} // 6
    };

    const bool kSchedulerStateAfterTrigger[kNumTriggers] = {true, true, true, true, true, true, false};

    const uint32_t kTimerHandlerCountAfterTrigger[kNumTriggers] = {1, 2, 3, 5, 7, 9, 10};

    const uint32_t kTimerStopCountAfterTrigger[kNumTriggers] = {0, 0, 0, 0, 0, 0, 1};

    const uint32_t kTimerStartCountAfterTrigger[kNumTriggers] = {3, 4, 5, 7, 9, 11, 11};

    size_t i;

    TestTimer *timers[kNumTimers];

    for (i = 0; i < kNumTimers; i++)
    {
        timers[i] = &aTimers[i];
    }

    // Start the Ten timers.

    InitTestTimer();
    InitCounters();

    for (i = 0; i < kNumTimers; i++)
    {
        sNow = kTimeT0[i] + aTimeShift;
        timers[i]->Start(kTimerInterval[i]);
    }

    // given the order in which timers are started,
    // the TimerScheduler should call vcNetPlatAlarmMilliStartAt 2 times.
    // one for timer[0] and one for timer[5] which will supercede timer[0].

    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], 2);
    EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], 0);
    EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], 0);
    EXPECT_EQ(sPlatT0, kTimeT0[5] + aTimeShift);
    EXPECT_EQ(sPlatDt, kTimerInterval[5]);
    EXPECT_TRUE(sTimerOn);

    for (i = 0; i < kNumTimers; i++)
    {
        EXPECT_TRUE(timers[i]->IsRunning());
    }

    // Issue the triggers and test the State after each trigger.

    for (size_t trigger = 0; trigger < kNumTriggers; trigger++)
    {
        sNow = kTriggerTimes[trigger] + aTimeShift;

        do
        {
            vcNetPlatAlarmMilliFired(aInstance);
        } while (sPlatDt == 0);

        EXPECT_EQ(sCallCount[kCallCountIndexAlarmStart], kTimerStartCountAfterTrigger[trigger]);
        EXPECT_EQ(sCallCount[kCallCountIndexAlarmStop], kTimerStopCountAfterTrigger[trigger]);
        EXPECT_EQ(sCallCount[kCallCountIndexTimerHandler], kTimerHandlerCountAfterTrigger[trigger]);
        EXPECT_TRUE(sTimerOn == kSchedulerStateAfterTrigger[trigger]);

        for (i = 0; i < kNumTriggers; i++)
        {
            EXPECT_TRUE(timers[i]->IsRunning() == kTimerStateAfterTrigger[trigger][i]);
        }
    }

    for (i = 0; i < kNumTriggers; i++)
    {
        EXPECT_EQ(timers[i]->GetFiredCounter(), 1);
    }

    for (i = 0; i < kNumTriggers; i++)
    {
        timers[i]->ResetFiredCounter();
    }
}

TEST_F(TestNetTimer, test_ten_timers)
{
    // Time shift to change the start/fire time of ten timers.
    const uint32_t kTimeShift[] = {
        0, 100000U, 0U - 1U, 0U - 1100U, Net::Timer::kMaxDelay, Net::Timer::kMaxDelay + 1020U,
    };

    const uint32_t kNumTimers = 10;


    TestTimer timer0;
    TestTimer timer1;
    TestTimer timer2;
    TestTimer timer3;
    TestTimer timer4;
    TestTimer timer5;
    TestTimer timer6;
    TestTimer timer7;
    TestTimer timer8;
    TestTimer timer9;

    TestTimer timers[kNumTimers] = {timer0, timer1, timer2, timer3, timer4,
                                    timer5, timer6, timer7, timer8, timer9};

    for (size_t i = 0; i < VC_ARRAY_LENGTH(kTimeShift); i++)
    {
        TenTimers(obj, kTimeShift[i], timers, kNumTimers);
    }
}

TEST_F(TestNetTimer, test_timer_time)
{
    const uint32_t kMaxTime = 0xffffffff;
    const uint32_t kStartTimes[] = {0, 100, kMaxTime / 2, kMaxTime - 100, kMaxTime};
    const uint32_t kDurations[] = {1, 100, Net::Timer::kMaxDelay - 1, Net::Timer::kMaxDelay};

    Net::Time t1;
    Net::Time t2;

    for (size_t i = 0; i < VC_ARRAY_LENGTH(kStartTimes); i++)
    {
        uint32_t start = kStartTimes[i];

        for (size_t j = 0; j < VC_ARRAY_LENGTH(kDurations); j++)
        {
            uint32_t duration = kDurations[j];

            t1.SetValue(start);

            EXPECT_EQ(t1.GetValue(), start);

            t2 = t1;

            EXPECT_EQ(t2.GetValue(), start);

            EXPECT_TRUE(t1 == t2);
            EXPECT_TRUE(!(t1 != t2));
            EXPECT_TRUE(!(t1 < t2));
            EXPECT_TRUE(t1 <= t2);
            EXPECT_TRUE(!(t1 > t2));
            EXPECT_TRUE(t1 >= t2);
            EXPECT_TRUE((t2 - t1) == 0);

            t2 = t1 + duration;

            EXPECT_TRUE(!(t1 == t2));
            EXPECT_TRUE(t1 != t2);
            EXPECT_TRUE(t1 < t2);
            EXPECT_TRUE(t1 <= t2);
            EXPECT_TRUE(!(t1 > t2));
            EXPECT_TRUE(!(t1 >= t2));
            EXPECT_TRUE((t2 - t1) == duration);

            t2 = t1;
            t2 += duration;

            EXPECT_TRUE(!(t1 == t2));
            EXPECT_TRUE(t1 != t2);
            EXPECT_TRUE(t1 < t2);
            EXPECT_TRUE(t1 <= t2);
            EXPECT_TRUE(!(t1 > t2));
            EXPECT_TRUE(!(t1 >= t2));
            EXPECT_TRUE((t2 - t1) == duration);

            t2 = t1 - duration;

            EXPECT_TRUE(!(t1 == t2));
            EXPECT_TRUE(t1 != t2);
            EXPECT_TRUE(!(t1 < t2));
            EXPECT_TRUE(!(t1 <= t2));
            EXPECT_TRUE(t1 > t2);
            EXPECT_TRUE(t1 >= t2);
            EXPECT_TRUE((t1 - t2) == duration);

            t2 = t1;
            t2 -= duration;

            EXPECT_TRUE(!(t1 == t2));
            EXPECT_TRUE(t1 != t2);
            EXPECT_TRUE(!(t1 < t2));
            EXPECT_TRUE(!(t1 <= t2));
            EXPECT_TRUE(t1 > t2);
            EXPECT_TRUE(t1 >= t2);
            EXPECT_TRUE((t1 - t2) == duration);
        }
    }
}
