#ifndef VCOS_CORE_SIRIUS_CONFIG_H
#define VCOS_CORE_SIRIUS_CONFIG_H

#define VCOS_CONFIG_PLATFORM_INFO "SIRIUS"

#ifndef VCOS_CONFIG_LOG_OUTPUT
#define VCOS_CONFIG_LOG_OUTPUT VCOS_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED
#endif

#ifndef VCOS_CONFIG_THREAD_EXTRA_STACKSIZE_PRINTF
#define VCOS_CONFIG_THREAD_EXTRA_STACKSIZE_PRINTF (512)
#endif

#ifndef VCOS_CONFIG_THREAD_STACKSIZE_DEFAULT
#define VCOS_CONFIG_THREAD_STACKSIZE_DEFAULT (1024)
#endif

#ifndef VCOS_CONFIG_THREAD_STACKSIZE_IDLE
#define VCOS_CONFIG_THREAD_STACKSIZE_IDLE (256)
#endif

#ifndef VCOS_CONFIG_THREAD_STACKSIZE_MAIN
#define VCOS_CONFIG_THREAD_STACKSIZE_MAIN (VCOS_CONFIG_THREAD_STACKSIZE_DEFAULT + VCOS_CONFIG_THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

#ifndef VCOS_CONFIG_TIMER_DEV
#define VCOS_CONFIG_TIMER_DEV TIM_DEV(0)
#define VCOS_CONFIG_TIMER_CHAN 0
#endif

#ifndef VCOS_CONFIG_TIMER_HZ
#define VCOS_CONFIG_TIMER_HZ (1000000)
#endif

#ifndef VCOS_CONFIG_TIMER_WIDTH
#define VCOS_CONFIG_TIMER_WIDTH 16
#endif

#endif /* VCOS_CORE_SIRIUS_CONFIG_H */
