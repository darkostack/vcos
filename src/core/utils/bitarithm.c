#include <stdio.h>

#include "core/utils/bitarithm.h"

unsigned bitarithmMsb(unsigned v)
{
    register unsigned r; /* result of log2(v) will go here */

    register unsigned shift;

    r =     (v > 0xFFFF) << 4; v >>= r;
    shift = (v > 0xFF  ) << 3; v >>= shift; r |= shift;
    shift = (v > 0xF   ) << 2; v >>= shift; r |= shift;
    shift = (v > 0x3   ) << 1; v >>= shift; r |= shift;
                                            r |= (v >> 1);

    return r;
}

unsigned bitarithmBitsSet(unsigned v)
{
    unsigned c; /* c accumulates the total bits set in v */

    for (c = 0; v; c++)
    {
        v &= v - 1; /* clear the least significant bit set */
    }

    return c;
}

const uint8_t MultiplyDeBruijnBitPosition[32] =
{
    0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
    31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};
