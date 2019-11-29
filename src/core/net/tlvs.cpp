#include "common/code_utils.hpp"

#include "net/message.hpp"
#include "net/tlvs.hpp"

namespace vc {
namespace Net {

uint32_t Tlv::GetSize(void) const
{
    return IsExtended() ? sizeof(ExtendedTlv) + static_cast<const ExtendedTlv *>(this)->GetLength()
                        : sizeof(Tlv) + GetLength();
}

uint8_t *Tlv::GetValue(void)
{
    return reinterpret_cast<uint8_t *>(this) + (IsExtended() ? sizeof(ExtendedTlv) : sizeof(Tlv));
}

const uint8_t *Tlv::GetValue(void) const
{
    return reinterpret_cast<const uint8_t *>(this) + (IsExtended() ? sizeof(ExtendedTlv) : sizeof(Tlv));
}

vcNetError Tlv::Get(const Message &aMessage, uint8_t aType, uint16_t aMaxSize, Tlv &aTlv)
{
    vcNetError error;
    uint16_t   offset;
    uint16_t   size;

    SuccessOrExit(error = Find(aMessage, aType, &offset, &size, NULL));

    if (aMaxSize > size)
    {
        aMaxSize = size;
    }

    aMessage.Read(offset, aMaxSize, &aTlv);

exit:
    return error;
}

vcNetError Tlv::GetOffset(const Message &aMessage, uint8_t aType, uint16_t &aOffset)
{
    return Find(aMessage, aType, &aOffset, NULL, NULL);
}

vcNetError Tlv::GetValueOffset(const Message &aMessage, uint8_t aType, uint16_t &aValueOffset, uint16_t &aLength)
{
    vcNetError error;
    uint16_t   offset;
    uint16_t   size;
    bool       isExtendedTlv;

    SuccessOrExit(error = Find(aMessage, aType, &offset, &size, &isExtendedTlv));

    if (!isExtendedTlv)
    {
        aValueOffset = offset + sizeof(Tlv);
        aLength      = size - sizeof(Tlv);
    }
    else
    {
        aValueOffset = offset + sizeof(ExtendedTlv);
        aLength      = size - sizeof(ExtendedTlv);
    }

exit:
    return error;
}

vcNetError Tlv::Find(const Message &aMessage, uint8_t aType, uint16_t *aOffset, uint16_t *aSize, bool *aIsExtendedTlv)
{
    vcNetError error        = VC_NET_ERROR_NOT_FOUND;
    uint16_t   offset       = aMessage.GetOffset();
    uint16_t   remainingLen = aMessage.GetLength();
    Tlv        tlv;
    uint32_t   size;

    VerifyOrExit(offset <= remainingLen);
    remainingLen -= offset;

    while (true)
    {
        VerifyOrExit(sizeof(Tlv) <= remainingLen);
        aMessage.Read(offset, sizeof(Tlv), &tlv);

        if (tlv.mLength != kExtendedLength)
        {
            size = tlv.GetSize();
        }
        else
        {
            ExtendedTlv extTlv;

            VerifyOrExit(sizeof(ExtendedTlv) <= remainingLen);
            aMessage.Read(offset, sizeof(ExtendedTlv), &extTlv);

            VerifyOrExit(extTlv.GetLength() <= (remainingLen - sizeof(ExtendedTlv)));
            size = extTlv.GetSize();
        }

        VerifyOrExit(size <= remainingLen);

        if (tlv.GetType() == aType)
        {
            if (aOffset != NULL)
            {
                *aOffset = offset;
            }

            if (aSize != NULL)
            {
                *aSize = static_cast<uint16_t>(size);
            }

            if (aIsExtendedTlv != NULL)
            {
                *aIsExtendedTlv = (tlv.mLength == kExtendedLength);
            }

            error = VC_NET_ERROR_NONE;
            ExitNow();
        }

        offset += size;
        remainingLen -= size;
    }

exit:
    return error;
}

} // namespace Net
} // namespace vc
