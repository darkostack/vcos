#ifndef VCOS_KERNEL_H
#define VCOS_KERNEL_H

#ifdef __cplusplus
extern "C" {
#endif

#define KERNEL_MAXTHREADS (32)

#define KERNEL_PID_UNDEF (0)
#define KERNEL_PID_FIRST (KERNEL_PID_UNDEF + 1)
#define KERNEL_PID_LAST (KERNEL_PID_FIRST + KERNEL_MAXTHREADS - 1)

#define PRIkernel_pid PRIi16

void vcKernelInit(void);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_KERNEL_H */
