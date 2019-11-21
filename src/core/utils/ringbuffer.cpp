#include <string.h>

#include "utils/ringbuffer.hpp"

namespace vc {
namespace Utils {

void RingBuffer::AddTail(char aChar)
{
    unsigned pos = mStart + mAvail++;

    if (pos >= mSize)
    {
        pos -= mSize;
    }

    mBuf[pos] = aChar;
}

char RingBuffer::GetHead(void)
{
    char result = mBuf[mStart];

    if ((--mAvail == 0) || (++mStart == mSize))
    {
        mStart = 0;
    }

    return result;
}

int RingBuffer::AddOne(char aChar)
{
    int result = -1;

    if (IsFull())
    {
        result = (unsigned char)GetHead();
    }

    AddTail(aChar);

    return result;
}

unsigned RingBuffer::Add(const char *aBuf, unsigned aSize)
{
    unsigned i;

    for (i = 0; i < aSize; i++)
    {
        if (IsFull())
        {
            break;
        }

        AddTail(aBuf[i]);
    }

    return i;
}

int RingBuffer::GetOne(void)
{
    if (!IsEmpty())
    {
        return (unsigned char)GetHead();
    }
    else
    {
        return -1;
    }
}

unsigned RingBuffer::Get(char *aBuf, unsigned aSize)
{
    if (aSize > mAvail)
    {
        aSize = mAvail;
    }

    if (aSize > 0)
    {
        unsigned bytesTillEnd = mSize - mStart;

        if (bytesTillEnd >= aSize)
        {
            memcpy(aBuf, mBuf + mStart, aSize);

            if (bytesTillEnd == aSize)
            {
                mStart = 0;
            }
            else
            {
                mStart += aSize;
            }
        }
        else
        {
            memcpy(aBuf, mBuf + mStart, bytesTillEnd);
            mStart = aSize - bytesTillEnd;
            memcpy(aBuf + bytesTillEnd, mBuf, mStart);
        }

        mAvail -= aSize;
    }

    return aSize;
}

unsigned RingBuffer::Remove(unsigned aSize)
{
    if (aSize > mAvail)
    {
        aSize  = mAvail;
        mStart = mAvail = 0;
    }
    else
    {
        mStart += aSize;
        mAvail -= aSize;

        /* compensate underflow */
        if (mStart > mSize)
        {
            mStart -= mSize;
        }
    }

    return aSize;
}

int RingBuffer::PeekOne(void)
{
    RingBuffer rb = *this;
    return rb.GetOne();
}

unsigned RingBuffer::Peek(char *aBuf, unsigned aSize)
{
    RingBuffer rb = *this;
    return rb.Get(aBuf, aSize);
}

} // namespace Utils
} // namespace vc
