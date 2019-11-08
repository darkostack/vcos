#include <stdio.h>
#include <assert.h>

void assert_failure(const char *file, unsigned line)
{
    printf("%s:%u => ASSERT FAILED\n", file, line); \
    while (1); \
}
