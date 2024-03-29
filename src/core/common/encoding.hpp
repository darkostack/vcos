#ifndef ENCODING_HPP_
#define ENCODING_HPP_

#include "vcos-core-config.h"

#include <limits.h>
#include <stdint.h>

namespace vc {
namespace Encoding {

inline uint16_t Swap16(uint16_t v)
{
    return (((v & 0x00ffU) << 8) & 0xff00) | (((v & 0xff00U) >> 8) & 0x00ff);
}

inline uint32_t Swap32(uint32_t v)
{
    return ((v & static_cast<uint32_t>(0x000000ffUL)) << 24) | ((v & static_cast<uint32_t>(0x0000ff00UL)) << 8) |
           ((v & static_cast<uint32_t>(0x00ff0000UL)) >> 8) | ((v & static_cast<uint32_t>(0xff000000UL)) >> 24);
}

inline uint64_t Swap64(uint64_t v)
{
    return ((v & static_cast<uint64_t>(0x00000000000000ffULL)) << 56) |
           ((v & static_cast<uint64_t>(0x000000000000ff00ULL)) << 40) |
           ((v & static_cast<uint64_t>(0x0000000000ff0000ULL)) << 24) |
           ((v & static_cast<uint64_t>(0x00000000ff000000ULL)) << 8) |
           ((v & static_cast<uint64_t>(0x000000ff00000000ULL)) >> 8) |
           ((v & static_cast<uint64_t>(0x0000ff0000000000ULL)) >> 24) |
           ((v & static_cast<uint64_t>(0x00ff000000000000ULL)) >> 40) |
           ((v & static_cast<uint64_t>(0xff00000000000000ULL)) >> 56);
}

inline uint32_t Reverse32(uint32_t v)
{
    v = ((v & 0x55555555) << 1) | ((v & 0xaaaaaaaa) >> 1);
    v = ((v & 0x33333333) << 2) | ((v & 0xcccccccc) >> 2);
    v = ((v & 0x0f0f0f0f) << 4) | ((v & 0xf0f0f0f0) >> 4);
    v = ((v & 0x00ff00ff) << 8) | ((v & 0xff00ff00) >> 8);
    v = ((v & 0x0000ffff) << 16) | ((v & 0xffff0000) >> 16);

    return v;
}

#define BitVectorBytes(x) static_cast<uint8_t>(((x) + (CHAR_BIT - 1)) / CHAR_BIT)

namespace BigEndian {

#if BYTE_ORDER_BIG_ENDIAN

inline uint16_t HostSwap16(uint16_t v)
{
    return v;
}
inline uint32_t HostSwap32(uint32_t v)
{
    return v;
}
inline uint64_t HostSwap64(uint64_t v)
{
    return v;
}

#else /* BYTE_ORDER_LITTLE_ENDIAN */

inline uint16_t HostSwap16(uint16_t v)
{
    return Swap16(v);
}
inline uint32_t HostSwap32(uint32_t v)
{
    return Swap32(v);
}
inline uint64_t HostSwap64(uint64_t v)
{
    return Swap64(v);
}

#endif // LITTLE_ENDIAN

inline uint16_t ReadUint16(const uint8_t *aBuffer)
{
    return static_cast<uint16_t>((aBuffer[0] << 8) | aBuffer[1]);
}

inline uint32_t ReadUint32(const uint8_t *aBuffer)
{
    return ((static_cast<uint32_t>(aBuffer[0]) << 24) | (static_cast<uint32_t>(aBuffer[1]) << 16) |
            (static_cast<uint32_t>(aBuffer[2]) << 8) | (static_cast<uint32_t>(aBuffer[3]) << 0));
}

inline void WriteUint16(uint16_t aValue, uint8_t *aBuffer)
{
    aBuffer[0] = (aValue >> 8) & 0xff;
    aBuffer[1] = (aValue >> 0) & 0xff;
}

inline void WriteUint32(uint32_t aValue, uint8_t *aBuffer)
{
    aBuffer[0] = (aValue >> 24) & 0xff;
    aBuffer[1] = (aValue >> 16) & 0xff;
    aBuffer[2] = (aValue >> 8) & 0xff;
    aBuffer[3] = (aValue >> 0) & 0xff;
}

} // namespace BigEndian

namespace LittleEndian {

#if BYTE_ORDER_BIG_ENDIAN

inline uint16_t HostSwap16(uint16_t v)
{
    return Swap16(v);
}
inline uint32_t HostSwap32(uint32_t v)
{
    return Swap32(v);
}
inline uint64_t HostSwap64(uint64_t v)
{
    return Swap64(v);
}

#else /* BYTE_ORDER_LITTLE_ENDIAN */

inline uint16_t HostSwap16(uint16_t v)
{
    return v;
}
inline uint32_t HostSwap32(uint32_t v)
{
    return v;
}
inline uint64_t HostSwap64(uint64_t v)
{
    return v;
}

#endif

inline uint16_t ReadUint16(const uint8_t *aBuffer)
{
    return static_cast<uint16_t>(aBuffer[0] | (aBuffer[1] << 8));
}

inline uint32_t ReadUint32(const uint8_t *aBuffer)
{
    return ((static_cast<uint32_t>(aBuffer[0]) << 0) | (static_cast<uint32_t>(aBuffer[1]) << 8) |
            (static_cast<uint32_t>(aBuffer[2]) << 16) | (static_cast<uint32_t>(aBuffer[3]) << 24));
}

inline void WriteUint16(uint16_t aValue, uint8_t *aBuffer)
{
    aBuffer[0] = (aValue >> 0) & 0xff;
    aBuffer[1] = (aValue >> 8) & 0xff;
}

inline void WriteUint32(uint32_t aValue, uint8_t *aBuffer)
{
    aBuffer[0] = (aValue >> 0) & 0xff;
    aBuffer[1] = (aValue >> 8) & 0xff;
    aBuffer[2] = (aValue >> 16) & 0xff;
    aBuffer[3] = (aValue >> 24) & 0xff;
}

} // namespace LittleEndian
} // namespace Encoding
} // namespace vc

#endif // ENCODING_HPP_
