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

class Isrpipe
{
public:
    Isrpipe(char *aBuf, unsigned int aSize)
        : mTsrb(aBuf, aSize)
        , mMutex()
    {
    }

    int WriteOne(char aChar);

    int Read(char *aBuf, size_t aCount);

    Tsrb &GetTsrb(void) { return mTsrb; }

    Mutex &GetMutex(void) { return mMutex; }

private:
    Tsrb mTsrb;
    Mutex mMutex;
};

} // namespace Utils
} // namespace vc

#endif /* UTILS_ISRPIPE_HPP */
