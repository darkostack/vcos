#include <stdio.h>
#include <assert.h>

void vcAssertFailure(const char *aFile, unsigned aLine)
{
    printf("%s:%u => ASSERT FAILED\r\n", aFile, aLine); \
    while (1); \
}
