#include <stdio.h>
#include <assert.h>

#include <vcos/instance.h>

#include "stdio_base.h"

vcInstance *gInstancePtr = NULL;

int main(int argc, char *argv[])
{
    gInstancePtr = vcInstanceInitSingle();

    assert(gInstancePtr != NULL);

    stdio_init();

    printf("Hello World!\n");

    char temp[16];
    while (1) {
        int res = stdio_read(&temp, 1);
        if (res != 0) {
            printf("%c\n", temp[0]);
        }
    }

    return 0;
}
