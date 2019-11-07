#ifndef NEW_HPP
#define NEW_HPP

#include <stddef.h>

inline void *operator new(size_t, void *p) throw()
{
    return p;
}

#endif // NEW_HPP_
