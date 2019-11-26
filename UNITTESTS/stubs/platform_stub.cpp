#include "test_platform.h"

#include <sys/time.h>

bool                 g_testPlatAlarmSet     = false;
uint32_t             g_testPlatAlarmNext    = 0;
testPlatAlarmStop    g_testPlatAlarmStop    = NULL;
testPlatAlarmStartAt g_testPlatAlarmStartAt = NULL;
testPlatAlarmGetNow  g_testPlatAlarmGetNow  = NULL;

void testNetPlatResetToDefaults(void)
{
    g_testPlatAlarmSet     = false;
    g_testPlatAlarmNext    = 0;
    g_testPlatAlarmStop    = NULL;
    g_testPlatAlarmStartAt = NULL;
    g_testPlatAlarmGetNow  = NULL;
}

extern "C" {

void vcNetPlatAlarmMilliStop(vcInstance *aInstance)
{
    if (g_testPlatAlarmStop)
    {
        g_testPlatAlarmStop(aInstance);
    }
    else
    {
        g_testPlatAlarmSet = false;
    }
}

void vcNetPlatAlarmMilliStartAt(vcInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    if (g_testPlatAlarmStartAt)
    {
        g_testPlatAlarmStartAt(aInstance, aT0, aDt);
    }
    else
    {
        g_testPlatAlarmSet  = true;
        g_testPlatAlarmNext = aT0 + aDt;
    }
}

uint32_t vcNetPlatAlarmMilliGetNow(void)
{
    if (g_testPlatAlarmGetNow)
    {
        return g_testPlatAlarmGetNow();
    }
    else
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (uint32_t)((tv.tv_sec * 1000) + (tv.tv_usec / 1000) + 123456);
    }
}

void vcNetPlatAlarmMicroStop(vcInstance *aInstance)
{
    if (g_testPlatAlarmStop)
    {
        g_testPlatAlarmStop(aInstance);
    }
    else
    {
        g_testPlatAlarmSet = false;
    }
}

void vcNetPlatAlarmMicroStartAt(vcInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    if (g_testPlatAlarmStartAt)
    {
        g_testPlatAlarmStartAt(aInstance, aT0, aDt);
    }
    else
    {
        g_testPlatAlarmSet  = true;
        g_testPlatAlarmNext = aT0 + aDt;
    }
}

uint32_t vcNetPlatAlarmMicroGetNow(void)
{
    if (g_testPlatAlarmGetNow)
    {
        return g_testPlatAlarmGetNow();
    }
    else
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (uint32_t)((tv.tv_sec * 1000000) + tv.tv_usec + 123456);
    }
}

}
