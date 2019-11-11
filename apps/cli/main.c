#include <stdio.h>
#include <assert.h>

int main(void)
{
    printf("hello world!\n");

    while (1) {
        printf("%c\n", getchar());
    }

    return 0;
}
