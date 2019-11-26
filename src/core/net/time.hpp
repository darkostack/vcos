#ifndef NET_TIME_HPP
#define NET_TIME_HPP

#include "vcos-core-config.h"

#include <stddef.h>
#include <stdint.h>

namespace vc {
namespace Net {

class Time
{
public:
    static const uint32_t kMaxDuration = ~static_cast<uint32_t>(0UL);

    Time(void) {}

   explicit Time(uint32_t aValue) { SetValue(aValue); }

   uint32_t GetValue(void) const { return mValue; }

   void SetValue(uint32_t aValue) { mValue = aValue; }

   uint32_t operator-(const Time &aOther) const { return mValue - aOther.mValue; }

   Time operator+(uint32_t aDuration) const { return Time(mValue + aDuration); }

   Time operator-(uint32_t aDuration) const { return Time(mValue - aDuration); }

   void operator+=(uint32_t aDuration) { mValue += aDuration; }

   void operator-=(uint32_t aDuration) { mValue -= aDuration; }

   bool operator==(const Time &aOther) const { return mValue == aOther.mValue; }

   bool operator!=(const Time &aOther) const { return !(*this == aOther); }

   bool operator<(const Time &aOther) const { return ((mValue - aOther.mValue) & (1UL << 31)) != 0; }

   bool operator>=(const Time &aOther) const { return !(*this < aOther); }

   bool operator<=(const Time &aOther) const { return (aOther >= *this); }

   bool operator>(const Time &aOther) const { return (aOther < *this); }

   static uint32_t SecToMsec(uint32_t aSeconds) { return aSeconds * 1000u; }

   static uint32_t MsecToSec(uint32_t aMilliseconds) { return aMilliseconds / 1000u; }

private:
    uint32_t mValue;
};

typedef Time TimeMilli;

typedef Time TimeMicro;

} // namespace Net
} // namespace vc

#endif /* NET_TIME_HPP */
