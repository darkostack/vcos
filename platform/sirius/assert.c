#include <stdio.h>
#include <assert.h>

void assert_failure(const char *file, unsigned line)
{
#if 0
    printf("%s:%u => ", file, line); \
#endif
    while (1);
}
