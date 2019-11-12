#ifndef VCOS_CORE_DEFAULT_CONFIG_H
#define VCOS_CORE_DEFAULT_CONFIG_H

#ifndef VCOS_CONFIG_PLATFORM_INFO
#define VCOS_CONFIG_PLATFORM_INFO "NONE"
#endif

#ifndef VCOS_CONFIG_LOG_OUTPUT
#define VCOS_CONFIG_LOG_OUTPUT VCOS_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED
#endif

#define VCOS_CONFIG_LOG_OUTPUT_NONE 0
#define VCOS_CONFIG_LOG_OUTPUT_DEBUG_UART 1
#define VCOS_CONFIG_LOG_OUTPUT_APP 2
#define VCOS_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED 3

#ifndef VCOS_CONFIG_TSRB_UART_MEMORY_SIZE
#define VCOS_CONFIG_TSRB_UART_MEMORY_SIZE 128
#endif

#ifndef VCOS_CONFIG_THREAD_STACKSIZE_DEFAULT
#error define VCOS_CONFIG_THREAD_STACKSIZE_DEFAULT must be defined per PLATFORM
#endif

#ifndef VCOS_CONFIG_THREAD_STACKSIZE_IDLE
#error define VCOS_CONFIG_THREAD_STACKSIZE_IDLE must be defined per PLATFORM
#endif

#ifndef VCOS_CONFIG_THREAD_EXTRA_STACKSIZE_PRINTF
#error VCOS_CONFIG_THREAD_EXTRA_STACKSIZE_PRINTF must be defined per PLATFORM
#endif

#ifndef VCOS_CONFIG_THREAD_STACKSIZE_MAIN
#define VCOS_CONFIG_THREAD_STACKSIZE_MAIN (VCOS_CONFIG_THREAD_STACKSIZE_DEFAULT + VCOS_CONFIG_THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

#ifndef VCOS_CONFIG_THREAD_STACKSIZE_LARGE
#define VCOS_CONFIG_THREAD_STACKSIZE_LARGE (VCOS_CONFIG_THREAD_STACKSIZE_MEDIUM * 2)
#endif

#ifndef VCOS_CONFIG_THREAD_STACKSIZE_MEDIUM
#define VCOS_CONFIG_THREAD_STACKSIZE_MEDIUM VCOS_CONFIG_THREAD_STACKSIZE_DEFAULT
#endif

#ifndef VCOS_CONFIG_THREAD_STACKSIZE_SMALL
#define VCOS_CONFIG_THREAD_STACKSIZE_SMALL (VCOS_CONFIG_THREAD_STACKSIZE_MEDIUM / 2)
#endif

#ifndef VCOS_CONFIG_THREAD_STACKSIZE_TINY
#define VCOS_CONFIG_THREAD_STACKSIZE_TINY (VCOS_CONFIG_THREAD_STACKSIZE_MEDIUM / 4)
#endif

#ifndef VCOS_CONFIG_THREAD_STACKSIZE_MINIMUM
#define VCOS_CONFIG_THREAD_STACKSIZE_MINIMUM (sizeof(Thread))
#endif

#ifndef VCOS_CONFIG_THREAD_PRIORITY_MIN
#define VCOS_CONFIG_THREAD_PRIORITY_MIN (VCOS_CONFIG_THREAD_SCHED_PRIO_LEVELS - 1)
#endif

#ifndef VCOS_CONFIG_THREAD_PRIORITY_IDLE
#define VCOS_CONFIG_THREAD_PRIORITY_IDLE VCOS_CONFIG_THREAD_PRIORITY_MIN
#endif

#ifndef VCOS_CONFIG_THREAD_PRIORITY_MAIN
#define VCOS_CONFIG_THREAD_PRIORITY_MAIN (VCOS_CONFIG_THREAD_PRIORITY_MIN - (VCOS_CONFIG_THREAD_SCHED_PRIO_LEVELS / 2))
#endif

#ifndef VCOS_CONFIG_THREAD_SCHED_PRIO_LEVELS
#define VCOS_CONFIG_THREAD_SCHED_PRIO_LEVELS (16)
#endif

#ifndef VCOS_CONFIG_CLI_MAX_LINE_LENGTH
#define VCOS_CONFIG_CLI_MAX_LINE_LENGTH 128
#endif

#ifndef VCOS_CONFIG_CLI_UART_TX_BUFFER_SIZE
#define VCOS_CONFIG_CLI_UART_TX_BUFFER_SIZE 1024
#endif

#ifndef VCOS_CONFIG_CLI_UART_RX_BUFFER_SIZE
#define VCOS_CONFIG_CLI_UART_RX_BUFFER_SIZE 512
#endif

#endif /* VCOS_CORE_DEFAULT_CONFIG_H */
