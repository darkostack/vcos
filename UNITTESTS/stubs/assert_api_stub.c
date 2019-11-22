#include <assert.h>
#include <stdio.h>

void vcAssertFailure(const char *aFile, unsigned aLine)
{
    (void) aFile;
    (void) aLine;

    return;
}
