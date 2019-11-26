#include <vcos/net/platform/alarm-micro.h>
#include <vcos/net/platform/alarm-milli.h>

#include <vcos/timer.h>

#define MS_PER_S 1000
#define NS_PER_US 1000
#define US_PER_MS 1000
#define US_PER_S 1000000

static bool sIsMsRunning = false;
static uint32_t sMsAlarm = 0;

static bool sIsUsRunning = false;
static uint32_t sUsAlarm = 0;

void vcNetPlatAlarmMilliStartAt(vcInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    (void) aInstance;

    sMsAlarm = aT0 + aDt;
    sIsMsRunning = true;
}

void vcNetPlatAlarmMilliStop(vcInstance *aInstance)
{
    (void) aInstance;

    sIsMsRunning = false;
}

uint32_t vcNetPlatAlarmMilliGetNow(void)
{
    return (uint32_t)(vcTimerNowUsec64() / US_PER_MS);
}

void vcNetPlatAlarmMicroStartAt(vcInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    (void) aInstance;

    sUsAlarm = aT0 + aDt;
    sIsUsRunning = true;
}

void vcNetPlatAlarmMicroStop(vcInstance *aInstance)
{
    (void) aInstance;

    sIsUsRunning = false;
}

uint32_t vcNetPlatAlarmMicroGetNow(void)
{
    return vcTimerNowUsec();
}

void vcNetPlatAlarmProcess(vcInstance *aInstance)
{
    int32_t remaining;

    if (sIsMsRunning)
    {
        remaining = (int32_t)(sMsAlarm - vcNetPlatAlarmMilliGetNow());

        if (remaining <= 0)
        {
            sIsMsRunning = false;

            vcNetPlatAlarmMilliFired(aInstance);
        }
    }

    if (sIsUsRunning)
    {
        remaining = (int32_t)(sUsAlarm - vcNetPlatAlarmMicroGetNow());

        if (remaining <= 0)
        {
            sIsUsRunning = false;

            vcNetPlatAlarmMicroFired(aInstance);
        }
    }
}
