#ifndef UTILS_ISRPIPE_HPP
#define UTILS_ISRPIPE_HPP

#include "vcos-core-config.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "common/mutex.hpp"
#include "common/code_utils.hpp"

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

    int Avail(void) { return mWrites - mReads; }

    bool IsEmpty(void) const { return mReads == mWrites; }

    bool IsFull(void) const { return (mWrites - mReads) == mSize; }

    int Free(void) { return (mSize - mWrites + mReads); }

    void Push(char aChar) { mBuf[mWrites++ & (mSize -1)] = aChar; }

    char Pop(void) { return mBuf[mReads++ & (mSize - 1)]; }

private:
    char *mBuf;
    unsigned int mSize;
    volatile unsigned mReads;
    volatile unsigned mWrites;
};

class Isrpipe
{
public:
    Isrpipe(char *aBuf, unsigned int aSize)
        : mMutex()
        , mTsrb(aBuf, aSize)
    {
    }

    int WriteOne(char aChar);

    int Read(char *aBuf, size_t aCount);

    Mutex &GetMutex(void) { return mMutex; }

    Tsrb &GetTsrb(void) { return mTsrb; }

private:
    Mutex mMutex;
    Tsrb mTsrb;
};

class UartIsrpipe : public Isrpipe
{
public:
    UartIsrpipe(void)
        : Isrpipe(static_cast<char *>(&mBuf[0]), VCOS_CONFIG_TSRB_UART_MEMORY_SIZE)
    {
    }

private:
    char mBuf[VCOS_CONFIG_TSRB_UART_MEMORY_SIZE];
};

} // namespace Utils
} // namespace vc

#endif /* UTILS_ISRPIPE_HPP */
