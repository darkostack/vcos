#include <assert.h>

#include "utils/isrpipe.hpp"

namespace vc {
namespace Utils {

int Isrpipe::WriteOne(char aChar)
{
    int res = AddOne(aChar);

    /* `res` is either 0 on success or -1 when the buffer is full. Either way,
     * unlocking the mutex is fine.
     */
    Unlock();

    return res;
}

int Isrpipe::Read(char *aBuf, size_t aCount)
{
    int res;
    while (!(res = Get(aBuf, aCount))) {
        Lock();
    }
    return res;
}

int Tsrb::GetOne(void)
{
    if (!IsEmpty()) {
        return Pop();
    } else {
        return -1;
    }
}

int Tsrb::Get(char *aBuf, size_t aCount)
{
    size_t tmp = aCount;
    while (tmp && !IsEmpty()) {
        *aBuf++ = Pop();
        tmp--;
    }
    return (aCount - tmp);
}

int Tsrb::Drop(size_t aCount)
{
    size_t tmp = aCount;
    while (tmp && !IsEmpty()) {
        Pop();
        tmp--;
    }
    return (aCount - tmp);
}

int Tsrb::AddOne(char aChar)
{
    if (!IsFull()) {
        Push(aChar);
        return 0;
    } else {
        return -1;
    }
}

int Tsrb::Add(const char *aBuf, size_t aSize)
{
    size_t tmp = aSize;
    while (tmp && !IsFull()) {
        Push(*aBuf++);
        tmp--;
    }
    return (aSize - tmp);
}

} // namespace Utils
} // namespace vc
