#include <stdint.h>
#include <stdbool.h>

#include <vcos/net/platform/alarm-milli.h>
#include <vcos/net/platform/alarm-micro.h>

#include <vcos/instance.h>

typedef void (*testPlatAlarmStop)(vcInstance *);
typedef void (*testPlatAlarmStartAt)(vcInstance *, uint32_t, uint32_t);
typedef uint32_t (*testPlatAlarmGetNow)(void);

extern bool                 g_testPlatAlarmSet;
extern uint32_t             g_testPlatAlarmNext;
extern testPlatAlarmStop    g_testPlatAlarmStop;
extern testPlatAlarmStartAt g_testPlatAlarmStartAt;
extern testPlatAlarmGetNow  g_testPlatAlarmGetNow;

void testNetPlatResetToDefaults(void);
