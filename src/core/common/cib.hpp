#ifndef CIB_HPP
#define CIB_HPP

#include <assert.h>
#include <stdint.h>

#include <vcos/types.h>

namespace vc {

/**
 * Circular integer buffer class.
 */
class Cib : public vcCib
{
public:
    explicit Cib(unsigned int aSize) { Init(aSize); }

    void Init(unsigned int aSize)
    {
        assert(!(aSize & (aSize - 1)));
        mReadCount  = 0;
        mWriteCount = 0;
        mMask       = aSize - 1;
    }

    int Full(void) { return (static_cast<int>(Avail()) > static_cast<int>(mMask)); }

    int Get(void)
    {
        if (Avail())
        {
            return static_cast<int>(mReadCount++ & mMask);
        }
        return -1;
    }

    int Peek(void)
    {
        if (Avail())
        {
            return static_cast<int>(mReadCount & mMask);
        }
        return -1;
    }

    int GetUnsafe(void) { return static_cast<int>(mReadCount++ & mMask); }

    int Put(void)
    {
        int avail = Avail();

        if (avail <= static_cast<int>(mMask))
        {
            return static_cast<int>(mWriteCount++ & mMask);
        }

        return -1;
    }

    int PutUnsafe(void) { return static_cast<int>(mWriteCount++ & mMask); }

    unsigned int Avail(void) { return mWriteCount - mReadCount; }

    unsigned int GetReadCount(void) { return mReadCount; }

    unsigned int GetWriteCount(void) { return mWriteCount; }

    unsigned int GetMask(void) { return mMask; }
};

} // namespace vc

#endif /* CIB_HPP */
