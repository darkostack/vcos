#ifndef UTILS_ISRPIPE_HPP
#define UTILS_ISRPIPE_HPP

#include "vcos-core-config.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "common/code_utils.hpp"
#include "common/locator.hpp"

namespace vc {
namespace Utils {

class Tsrb
{
public:
    Tsrb(char *aBuf, unsigned int aSize)
        : mBuf(aBuf)
        , mSize(aSize)
        , mReads(0)
        , mWrites(0)
    {
        assert((aSize != 0) && ((aSize & (~aSize + 1)) == aSize));
    }

    int GetOne(void);

    int Get(char *aBuf, size_t aCount);

    int Drop(size_t aCount);

    int AddOne(char aChar);

    int Add(const char *aBuf, size_t aSize);

private:
    bool IsEmpty(void) const { return mReads == mWrites; }
    int Avail(void) { return mWrites - mReads; }
    bool IsFull(void) const { return (mWrites - mReads) == mSize; }
    int Free(void) { return (mSize - mWrites + mReads); }
    void Push(char aChar) { mBuf[mWrites++ & (mSize -1)] = aChar; }
    char Pop(void) { return mBuf[mReads++ & (mSize - 1)]; }

    char *mBuf;
    unsigned int mSize;
    volatile unsigned mReads;
    volatile unsigned mWrites;
};

class Isrpipe : public Tsrb
{
public:
    Isrpipe(char *aBuf, unsigned int aSize)
        : Tsrb(aBuf, aSize)
    {
    }

    int WriteOne(char aChar);

    int Read(char *aBuf, size_t aCount);
};

class IsrpipeUart : public InstanceLocator, public Isrpipe
{
public:
    IsrpipeUart(Instance &aInstance)
        : InstanceLocator(aInstance)
        , Isrpipe(static_cast<char*>(&mBuf[0]), static_cast<unsigned int>(VC_ARRAY_LENGTH(mBuf)))
    {
    }

private:
    char mBuf[VCOS_CONFIG_TSRB_UART_MEMORY_SIZE];
};

} // namespace Utils
} // namespace vc

#endif /* UTILS_ISRPIPE_HPP */
