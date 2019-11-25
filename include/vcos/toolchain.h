#ifndef VCOS_TOOLCHAIN_H
#define VCOS_TOOLCHAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Toolchain related defines.
 */
#ifdef __GNUC__
#define container_of(PTR, TYPE, MEMBER)                                       \
    (__extension__({                                                          \
        __extension__ const __typeof__(((TYPE *)0)->MEMBER) *__m____ = (PTR); \
        ((TYPE *)((char *)__m____ - offsetof(TYPE, MEMBER)));                 \
    }))
#else
#define container_of(PTR, TYPE, MEMBER) ((TYPE *)((char *)(PTR)-offsetof(TYPE, MEMBER)))
#endif

#ifdef __GNUC__
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

#ifdef __GNUC__
#define CONST __attribute__((const))
#else
#define CONST
#endif

#ifdef __GNUC__
#define PURE __attribute__((pure))
#else
#define PURE
#endif

#if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ >= 5)
#define UNREACHABLE() __builtin_unreachable()
#else
#define UNREACHABLE() \
    do                \
    { /* nothing */   \
    } while (1)
#endif

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2 * !!(condition)]))

#ifndef SSIZE_MAX
#define SSIZE_MAX ((ssize_t)(SIZE_MAX / 2))
#endif

#ifdef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

#ifdef __cplusplus
}
#endif

#endif /* VCOS_TOOLCHAIN_H */
