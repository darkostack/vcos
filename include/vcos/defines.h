#ifndef VCOS_DEFINES_H
#define VCOS_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

#define KERNEL_MAXTHREADS (32)

#define KERNEL_PID_UNDEF (0)
#define KERNEL_PID_FIRST (KERNEL_PID_UNDEF + 1)
#define KERNEL_PID_LAST  (KERNEL_PID_FIRST + KERNEL_MAXTHREADS - 1)

#define PRIkernel_pid PRIi16

#define MUTEX_LOCKED ((vcListNode *)-1)

/**
 * Optional flags for controlling a threads initial state.
 */
#define THREAD_FLAGS_CREATE_SLEEPING (1)
#define THREAD_FLAGS_AUTO_FREE (2)
#define THREAD_FLAGS_CREATE_WOUT_YIELD (4)
#define THREAD_FLAGS_CREATE_STACKTEST (8)

#define THREAD_STATUS_ON_RUNQUEUE THREAD_STATUS_RUNNING
#define THREAD_STATUS_NOT_FOUND ((vcThreadStatus)-1)

/**
 * This marker is used e.g. by 'Thread::SwitchContextExit()' to identify the stacks
 * beginning.
 */
#define STACK_MARKER (0x77777777)

/**
 * Initial program status register value for a newly created thread
 */ 
#define INITIAL_XPSR (0x01000000)

/**
 * ARM Cortex-M specific exception return value, that triggers the return to the
 * task mode stack pointer.
 */
#define EXCEPT_RET_TASK_MODE (0xfffffffd)

/**
 * Toolchain related defines.
 */
#ifdef __GNUC__
#define container_of(PTR, TYPE, MEMBER) \
        (__extension__ ({ \
            __extension__ const __typeof__ (((TYPE *) 0)->MEMBER) *__m____ = (PTR); \
            ((TYPE *) ((char *) __m____ - offsetof(TYPE, MEMBER))); \
        }))
#else
#define container_of(PTR, TYPE, MEMBER) \
        ((TYPE *) ((char *) (PTR) - offsetof(TYPE, MEMBER)))
#endif

#ifdef __GNUC__
#define NORETURN  __attribute__((noreturn))
#else
#define NORETURN
#endif

#ifdef __GNUC__
#define CONST  __attribute__((const))
#else
#define CONST
#endif

#ifdef __GNUC__
#define PURE  __attribute__((pure))
#else
#define PURE
#endif

#if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ >= 5)
#define UNREACHABLE() __builtin_unreachable()
#else
#define UNREACHABLE() do { /* nothing */ } while (1)
#endif

#define ALIGN_OF(T) (offsetof(struct { char c; T t; }, t))

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2 * !!(condition)]))

#ifndef SSIZE_MAX
#define SSIZE_MAX ((ssize_t) (SIZE_MAX / 2))
#endif

#ifdef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

#ifdef __cplusplus
}
#endif

#endif /* VCOS_DEFINES_H */
