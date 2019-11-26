#ifndef UTILS_STATIC_ASSERT_HPP
#define UTILS_STATIC_ASSERT_HPP

#ifdef __cplusplus

#ifndef __COUNTER__
#define __COUNTER__ __LINE__
#endif

namespace vc {

template <int> struct StaticAssertError;
template <> struct StaticAssertError<true>
{
    StaticAssertError(...);
};

} // namespace vc

#define __VC_STATIC_ASSERT_ERROR(aError, aLine) aError##aLine
#define _VC_STATIC_ASSERT_ERROR(aLine) __VC_STATIC_ASSERT_ERROR(StaticAssertError, aLine)

#define VC_STATIC_ASSERT(aExpression, aMessage)                                                             \
    enum                                                                                                    \
    {                                                                                                       \
        _VC_STATIC_ASSERT_ERROR(__COUNTER__) = sizeof(vc::StaticAssertError<(aExpression) != 0>(aMessage)), \
    }

#endif // __cplusplus

#endif /* UTILS_STATIC_ASSERT_HPP */
