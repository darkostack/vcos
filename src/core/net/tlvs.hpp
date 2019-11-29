#ifndef NET_TLVS_HPP
#define NET_TLVS_HPP

#include "vcos-core-config.h"

#include <string.h>

#include <vcos/net/error.h>

#include "common/encoding.hpp"

namespace vc {
namespace Net {

using vc::Encoding::BigEndian::HostSwap16;
using vc::Encoding::BigEndian::HostSwap32;

class Message;

class Tlv
{
public:
    enum
    {
        kBaseTlvMaxLength = 254,
    };

    uint8_t GetType(void) const { return mType; }

    void SetType(uint8_t aType) { mType = aType; }

    bool IsExtended(void) const { return (mLength == kExtendedLength); }

    uint8_t GetLength(void) const { return mLength; }

    void SetLength(uint8_t aLength) { mLength = aLength; }

    uint32_t GetSize(void) const;

    uint8_t *GetValue(void);

    const uint8_t *GetValue(void) const;

    Tlv *GetNext(void) { return reinterpret_cast<Tlv *>(reinterpret_cast<uint8_t *>(this) + GetSize()); }

    const Tlv *GetNext(void) const
    {
        return reinterpret_cast<const Tlv *>(reinterpret_cast<const uint8_t *>(this) + GetSize());
    }

    static vcNetError Get(const Message &aMessage, uint8_t aType, uint16_t aMaxSize, Tlv &aTlv);

    static vcNetError GetOffset(const Message &aMessage, uint8_t aType, uint16_t &aOffset);

    static vcNetError GetValueOffset(const Message &aMessage, uint8_t aType, uint16_t &aOffset, uint16_t &aLength);

protected:
    enum
    {
        kExtendedLength = 255,
    };

private:
    static vcNetError Find(const Message &aMessage,
                           uint8_t        aType,
                           uint16_t *     aOffset,
                           uint16_t *     aSize,
                           bool *         aIsExtededTlv);

    uint8_t mType;
    uint8_t mLength;
} __attribute__((packed));

class ExtendedTlv : public Tlv
{
public:
    uint16_t GetLength(void) const { return HostSwap16(mLength); }

    void SetLength(uint16_t aLength)
    {
        Tlv::SetLength(kExtendedLength);
        mLength = HostSwap16(aLength);
    }

private:
    uint16_t mLength;
} __attribute__((packed));

} // namespace Net
} // namespace vc

#endif /* NET_TLVS_HPP */
