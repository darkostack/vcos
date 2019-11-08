#include <stdio.h>
#include <assert.h>

void assertFailure(const char *aFile, unsigned aLine)
{
    printf("%s:%u => ASSERT FAILED\n", aFile, aLine); \
    while (1); \
}
