#include <stdio.h>
#include <assert.h>

#include <vcos/instance.h>
#include <vcos/cli.h>
#include <vcos/timer.h>

static void timer1Callback(void *aArg)
{
    vcTimer *timer = (vcTimer *)aArg;
    printf("timer1Callback() now: %lu\r\n", vcTimerNow().mTicks32);
    vcTimerSet(timer, 1000000);
}

static void timer2Callback(void *aArg)
{
    vcTimer *timer = (vcTimer *)aArg;
    printf("timer2Callback() now: %lu\r\n", vcTimerNow().mTicks32);
    vcTimerSet(timer, 500000);
}

int main(void)
{
    vcInstance *instance = vcInstanceGet();

    assert(vcInstanceIsInitialized(instance));

    vcTimer timer1;
    vcTimer timer2;

    vcTimerInit(&timer1, timer1Callback, (void *)&timer1);
    vcTimerInit(&timer2, timer2Callback, (void *)&timer2);

    vcTimerSet(&timer1, 1000000);
    vcTimerSet(&timer2, 500000);

    vcCliUartInit(instance);

    vcCliUartRun();

    /* Should not reach here */
    assert(0);

    return 0;
}
