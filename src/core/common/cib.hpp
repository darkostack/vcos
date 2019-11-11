#ifndef CIB_HPP
#define CIB_HPP

#include <stdint.h>
#include <assert.h>

namespace vc {

/**
 * Circular integer buffer class.
 */
class Cib
{
public:
    Cib(unsigned int aSize)
        : mReadCount(0)
        , mWriteCount(0)
        , mMask(aSize - 1)
    {
        assert(!(aSize & (aSize - 1)));
    }

    int Full(void) { return (static_cast<int>(Avail()) > static_cast<int>(mMask)); }

    int Get(void)
    {
        if (Avail()) {
            return static_cast<int>(mReadCount++ & mMask);
        }
        return -1;
    }

    int Peek(void)
    {
        if (Avail()) {
            return static_cast<int>(mReadCount & mMask);
        }
        return -1;
    }

    int GetUnsafe(void) { return static_cast<int>(mReadCount++ & mMask); }

    int Put(void)
    {
        int avail = Avail();
        if (avail <= static_cast<int>(mMask)) {
            return static_cast<int>(mWriteCount++ & mMask);
        }
        return -1;
    }

    int PutUnsafe(void) { return static_cast<int>(mWriteCount++ & mMask); }

private:
    unsigned int Avail(void) { return mWriteCount - mReadCount; }

    unsigned int mReadCount;
    unsigned int mWriteCount;
    unsigned int mMask;
};

} // namespace vc

#endif /* CIB_HPP */
