#ifndef UTILS_RINGBUFFER_HPP
#define UTILS_RINGBUFFER_HPP

#include "vcos-core-config.h"

#include <vcos/types.h>

#include "common/code_utils.hpp"

namespace vc {
namespace Utils {

class RingBuffer : public vcRingBuffer
{
public:
    void Init(char *aBuffer, unsigned aSize)
    {
        mBuf   = aBuffer;
        mSize  = aSize;
        mStart = 0;
        mAvail = 0;
    }

    int AddOne(char aChar);

    unsigned Add(const char *aBuf, unsigned aSize);

    int GetOne(void);

    unsigned Get(char *aBuf, unsigned aSize);

    unsigned Remove(unsigned aSize);

    int IsEmpty(void) { return mAvail == 0; }

    int IsFull(void) { return mAvail == mSize; }

    unsigned int Free(void) { return mSize - mAvail; }

    unsigned int Avail() { return mAvail; }

    int PeekOne(void);

    unsigned Peek(char *aBuf, unsigned aSize);

private:
    void AddTail(char aChar);

    char GetHead(void);
};

} // namespace Utils
} // namespace vc

#endif /* UTILS_RINGBUFFER_HPP */
