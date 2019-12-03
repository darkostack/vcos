#ifndef NET_MAC_FRAME_HPP
#define NET_MAC_FRAME_HPP

#include "vcos-core-config.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>

#include <vcos/net/platform/radio.h>

#include "common/encoding.hpp"

#include "net/string.hpp"

namespace vc {
namespace Net {
namespace Mac {

enum
{
    kShortAddrBroadcast = 0xffff,
    kShortAddrInvalid   = 0xfffe,
    kPanIdBroadcast     = 0xffff,
};

typedef vcNetPanId PanId;

typedef vcNetShortAddress ShortAddress;

class ExtAddress : public vcNetExtAddress
{
public:
    enum
    {
        kInfoStringSize = 17,
    };

    typedef String<kInfoStringSize> InfoString;

    enum CopyByteOrder
    {
        kNormalByteOrder,
        kReverseByteOrder,
    };

    void GenerateRandom(void);

    void Set(const uint8_t *aBuffer, CopyByteOrder aByteOrder = kNormalByteOrder)
    {
        CopyAddress(m8, aBuffer, aByteOrder);
    }

    bool IsGroup(void) const { return (m8[0] & kGroupFlag) != 0; }

    void SetGroup(bool aGroup)
    {
        if (aGroup)
        {
            m8[0] |= kGroupFlag;
        }
        else
        {
            m8[0] &= ~kGroupFlag;
        }
    }

    void ToggleGroup(void) { m8[0] ^= kGroupFlag; }

    bool IsLocal(void) const { return (m8[0] & kLocalFlag) != 0; }

    void SetLocal(bool aLocal)
    {
        if (aLocal)
        {
            m8[0] |= kLocalFlag;
        }
        else
        {
            m8[0] &= ~kLocalFlag;
        }
    }

    void ToggleLocal(void) { m8[0] ^= kLocalFlag; }

    void CopyTo(uint8_t *aBuffer, CopyByteOrder aByteOrder = kNormalByteOrder) const
    {
        CopyAddress(aBuffer, m8, aByteOrder);
    }

    bool operator==(const ExtAddress &aOther) const;

    bool operator!=(const ExtAddress &aOther) const { return !(*this == aOther); }

    InfoString ToString(void) const;

private:
    static void CopyAddress(uint8_t *aDst, const uint8_t *aSrc, CopyByteOrder aByteOrder);

    enum
    {
        kGroupFlag = 1 << 0,
        kLocalFlag = 1 << 1,
    };
} __attribute__((packed))

class Address
{
public:
    typedef ExtAddress::InfoString InfoString;

    enum Type
    {
        kTypeNone,     // No address.
        kTypeShort,    // IEEE 802.15.4 Short Address
        kTypeExtended, // IEEE 802.15.4 Extended Address
    };

    Address(void)
        : mType(kTypeNone)
    {
    }

    Type GetType(void) const { return mType; }

    bool IsNone(void) const { return (mType == kTypeNone); }

    bool IsShort(void) const { return (mType == kTypeShort); }

    bool IsExtended(void) const { return (mType == kTypeExtended); }

    ShortAddress GetShort(void) const { return mShared.mShortAddress; }

    const ExtAddress &GetExtended(void) const { return mShared.mExtAddress; }

    ExtAddress &GetExtended(void) { return mShared.mExtAddress; }

    void SetNone(void) { mType = kTypeNone; }

    void SetShort(ShortAddress aShortAddress)
    {
        mShared.mShortAddress = aShortAddress;
        mType                 = kTypeShort;
    }

    void SetExtended(const ExtAddress &aExtAddress)
    {
        mShared.mExtAddress = aExtAddress;
        mType               = kTypeExtended;
    }

    void SetExtended(const uint8_t *aBuffer, ExtAddress::CopyByteOrder aByteOrder = ExtAddress::kNormalByteOrder)
    {
        mShared.mExtAddress.Set(aBuffer, aByteOrder);
        mType = kTypeExtended;
    }

    bool IsBroadcast(void) const { return ((mType == kTypeShort) && (GetShort() == kShortAddrBroadcast)); }

    bool IsShortAddrInvalid(void) const { return ((mType == kTypeShort) && (GetShort() == kShortAddrInvalid)); }

    InfoString ToString(void) const;

private:
    union
    {
        ShortAddress mShortAddress; // The IEEE 802.15.4 Short Address.
        ExtAddress   mExtAddress;   // The IEEE 802.15.4 Extended Address.
    } mShared;

    Type mType;
};

class ExtendedPanId : public vcNetExtendedPanId
{
public:
    enum
    {
        kInfoStringSize = 17,
    };

    typedef String<kInfoStringSize> InfoString;

    bool operator==(const ExtendedPanId &aOther) const;

    bool operator!=(const ExtendedPanId &aOther) const { return !(*this == aOther); }

    InfoString ToString(void) const;
} __attribute__((packed));

class NetworkName : public vcNetNetworkName
{
public:
    enum
    {
        kMaxSize = VC_NET_NETWORK_NAME_MAX_SIZE,
    };

    class Data
    {
    public:
        Data(const char *aBuffer, uint8_t aLength)
            : mBuffer(aBuffer)
            , mLength(aLength)
        {
        }

        const char *GetBuffer(void) const { return mBuffer; }

        uint8_t GetLength(void) const { return mLength; }

        uint8_t CopyTo(char *aBuffer, uint8_t aMaxSize) const;

    private:
        const char *mBuffer;
        uint8_t     mLength;
    };

    NetworkName(void) { m8[0] = '\0'; }

    const char *GetAsCString(void) const { return m8; }

    Data GetAsData(void) const;

    vcNetError Set(const Data &aNameData);
};

class Frame : public vcNetRadioFrame
{
public:
    enum
    {
        kMtu                 = VC_NET_RADIO_FRAME_MAX_SIZE,
        kFcfSize             = sizeof(uint16_t),
        kDsnSize             = sizeof(uint8_t),
        kSecurityControlSize = sizeof(uint8_t),
        kFrameCounterSize    = sizeof(uint32_t),
        kCommandIdSize       = sizeof(uint8_t),
        kFcsSize             = sizeof(uint16_t),

        kFcfFrameBeacon      = 0 << 0,
        kFcfFrameData        = 1 << 0,
        kFcfFrameAck         = 2 << 0,
        kFcfFrameMacCmd      = 3 << 0,
        kFcfFrameTypeMask    = 7 << 0,
        kFcfSecurityEnabled  = 1 << 3,
        kFcfFramePending     = 1 << 4,
        kFcfAckRequest       = 1 << 5,
        kFcfPanidCompression = 1 << 6,
        kFcfIePresent        = 1 << 9,
        kFcfDstAddrNone      = 0 << 10,
        kFcfDstAddrShort     = 2 << 10,
        kFcfDstAddrExt       = 3 << 10,
        kFcfDstAddrMask      = 3 << 10,
        kFcfFrameVersion2006 = 1 << 12,
        kFcfFrameVersion2015 = 2 << 12,
        kFcfFrameVersionMask = 3 << 12,
        kFcfSrcAddrNone      = 0 << 14,
        kFcfSrcAddrShort     = 2 << 14,
        kFcfSrcAddrExt       = 3 << 14,
        kFcfSrcAddrMask      = 3 << 14,

        kSecNone      = 0 << 0,
        kSecMic32     = 1 << 0,
        kSecMic64     = 2 << 0,
        kSecMic128    = 3 << 0,
        kSecEnc       = 4 << 0,
        kSecEncMic32  = 5 << 0,
        kSecEncMic64  = 6 << 0,
        kSecEncMic128 = 7 << 0,
        kSecLevelMask = 7 << 0,

        kMic0Size   = 0,
        kMic32Size  = 32 / CHAR_BIT,
        kMic64Size  = 64 / CHAR_BIT,
        kMic128Size = 128 / CHAR_BIT,
        kMaxMicSize = kMic128Size,

        kKeyIdMode0    = 0 << 3,
        kKeyIdMode1    = 1 << 3,
        kKeyIdMode2    = 2 << 3,
        kKeyIdMode3    = 3 << 3,
        kKeyIdModeMask = 3 << 3,

        kKeySourceSizeMode0 = 0,
        kKeySourceSizeMode1 = 0,
        kKeySourceSizeMode2 = 4,
        kKeySourceSizeMode3 = 8,

        kKeyIndexSize = sizeof(uint8_t),

        kMacCmdAssociationRequest         = 1,
        kMacCmdAssociationResponse        = 2,
        kMacCmdDisassociationNotification = 3,
        kMacCmdDataRequest                = 4,
        kMacCmdPanidConflictNotification  = 5,
        kMacCmdOrphanNotification         = 6,
        kMacCmdBeaconRequest              = 7,
        kMacCmdCoordinatorRealignment     = 8,
        kMacCmdGtsRequest                 = 9,

        kHeaderIeVendor       = 0x00,
        kHeaderIeTermination2 = 0x7f,

        kInfoStringSize = 110, ///< Max chars needed for the info string representation (@sa ToInfoString()).
    };

    typedef String<kInfoStringSize> InfoString;

    void InitMacHeader(uint16_t aFcf, uint8_t aSecurityControl);

    vcNetError ValidatePsdu(void) const;

    uint8_t GetType(void) const { return GetPsdu()[0] & kFcfFrameTypeMask; }

    uint16_t GetVersion(void) const { return GetFrameControlField() & kFcfFrameVersionMask; }

    bool GetSecurityEnabled(void) const { return (GetPsdu()[0] & kFcfSecurityEnabled) != 0; }

    bool GetFramePending(void) const { return (GetPsdu()[0] & kFcfFramePending) != 0; }

    void SetFramePending(bool aFramePending);

    bool GetAckRequest(void) const { return (GetPsdu()[0] & kFcfAckRequest) != 0; }

    void SetAckRequest(bool aAckRequest);

    bool IsIePresent(void) const { return (GetFrameControlField() & kFcfIePresent) != 0; }

    uint8_t GetSequence(void) const { return GetPsdu()[kSequenceIndex]; }

    void SetSequence(uint8_t aSequence) { GetPsdu()[kSequenceIndex] = aSequence; }

    vcNetError GetDstPanId(PanId &aPanId) const;

    void SetDstPanId(PanId aPanId);

    vcNetError GetDstAddr(Address &aAddress) const;

    void SetDstAddr(ShortAddress aShortAddress);

    void SetDstAddr(const ExtAddress &aExtAddress);

    void SetDstAddr(const Address &aAddress);

    bool IsSrcPanIdPresent(uint16_t aFcf) const;

    vcNetError GetSrcPanId(PanId &aPanId) const;

    vcNetError SetSrcPanId(PanId aPanId);

    vcNetError GetSrcAddr(Address &aAddress) const;

    void SetSrcAddr(ShortAddress aShortAddress);

    void SetSrcAddr(const ExtAddress &aExtAddress);

    void SetSrcAddr(const Address &aAddress);

    vcNetError GetSecurityLevel(uint8_t &aSecurityLevel) const;

    vcNetError GetKeyIdMode(uint8_t &aKeyIdMode) const;

    vcNetError GetFrameCounter(uint32_t &aFrameCounter) const;

    void SetFrameCounter(uint32_t aFrameCounter);

    const uint8_t *GetKeySource(void) const;

    void SetKeySource(const uint8_t *aKeySource);

    vcNetError GetKeyId(uint8_t &aKeyId) const;

    void SetKeyId(uint8_t aKeyId);

    vcNetError GetCommandId(uint8_t &aCommandId) const;

    vcNetError SetCommandId(uint8_t aCommandId);

    bool IsDataRequestCommand(void) const;

    uint16_t GetLength(void) const { return GetPsduLength(); }

    void SetLength(uint16_t aLength) { SetPsduLength(aLength); }

    uint8_t GetHeaderLength(void) const;

    uint8_t GetFooterLength(void) const;

    uint16_t GetPayloadLength(void) const;

    uint16_t GetMaxPayloadLength(void) const;

    void SetPayloadLength(uint16_t aLength);

    uint8_t GetChannel(void) const { return mChannel; }

    void SetChannel(uint8_t aChannel) { mChannel = aChannel; }

    uint16_t GetPsduLength(void) const { return mLength; }

    void SetPsduLength(uint16_t aLength) { mLength = aLength; }

    uint8_t *GetPsdu(void) { return mPsdu; }

    const uint8_t *GetPsdu(void) const { return mPsdu; }

    uint8_t *GetHeader(void) { return GetPsdu(); }

    const uint8_t *GetHeader(void) const { return GetPsdu(); }

    uint8_t *GetPayload(void) { return const_cast<uint8_t *>(const_cast<const Frame *>(this)->GetPayload()); }

    const uint8_t *GetPayload(void) const;

    uint8_t *GetFooter(void) { return const_cast<uint8_t *>(const_cast<const Frame *>(this)->GetFooter()); }

    const uint8_t *GetFooter(void) const;

    uint16_t GetMtu(void) const { return kMtu; }

    uint16_t GetFcsSize(void) const { return kFcsSize; }

    InfoString ToInfoString(void) const;

private:
    enum
    {
        kInvalidIndex  = 0xff,
        kSequenceIndex = kFcfSize,
    };

    uint16_t GetFrameControlField(void) const;
    uint8_t  FindDstPanIdIndex(void) const;
    uint8_t  FindDstAddrIndex(void) const;
    uint8_t  FindSrcPanIdIndex(void) const;
    uint8_t  FindSrcAddrIndex(void) const;
    uint8_t  FindSecurityHeaderIndex(void) const;
    uint8_t  SkipSecurityHeaderIndex(void) const;
    uint8_t  FindPayloadIndex(void) const;

    static uint8_t GetKeySourceLength(uint8_t aKeyIdMode);
};

class RxFrame : public Frame
{
public:
    int8_t GetRssi(void) const { return mInfo.mRxInfo.mRssi; }

    void SetRssi(int8_t aRssi) { mInfo.mRxInfo.mRssi = aRssi; }

    uint8_t GetLqi(void) const { return mInfo.mRxInfo.mLqi; }

    void SetLqi(uint8_t aLqi) { mInfo.mRxInfo.mLqi = aLqi; }

    bool IsAckedWithFramePending(void) const { return mInfo.mRxInfo.mAckedWithFramePending; }

    const uint64_t &GetTimestamp(void) const { return mInfo.mRxInfo.mTimestamp; }
};

class TxFrame : public Frame
{
public:
    uint8_t GetMaxCsmaBackoffs(void) const { return mInfo.mTxInfo.mMaxCsmaBackoffs; }

    void SetMaxCsmaBackoffs(uint8_t aMaxCsmaBackoffs) { mInfo.mTxInfo.mMaxCsmaBackoffs = aMaxCsmaBackoffs; }

    uint8_t GetMaxFrameRetries(void) const { return mInfo.mTxInfo.mMaxFrameRetries; }

    void SetMaxFrameRetries(uint8_t aMaxFrameRetries) { mInfo.mTxInfo.mMaxFrameRetries = aMaxFrameRetries; }

    bool IsARetransmission(void) const { return mInfo.mTxInfo.mIsARetx; }

    void SetIsARetransmission(bool aIsARetx) { mInfo.mTxInfo.mIsARetx = aIsARetx; }

    bool IsCsmaCaEnabled(void) const { return mInfo.mTxInfo.mCsmaCaEnabled; }

    void SetCsmaCaEnabled(bool aCsmaCaEnabled) { mInfo.mTxInfo.mCsmaCaEnabled = aCsmaCaEnabled; }

    const uint8_t *GetAesKey(void) const { return mInfo.mTxInfo.mAesKey; }

    void SetAesKey(const uint8_t *aAesKey) { mInfo.mTxInfo.mAesKey = aAesKey; }

    void CopyFrom(const TxFrame &aFromFrame);
};

class Beacon
{
public:
    enum
    {
        kSuperFrameSpec = 0x0fff, // Superframe Specification value.
    };

    void Init(void)
    {
        mSuperframeSpec     = vc::Encoding::LittleEndian::HostSwap16(kSuperFrameSpec);
        mGtsSpec            = 0;
        mPendingAddressSpec = 0;
    }

    bool IsValid(void) const
    {
        return (mSuperframeSpec == vc::Encoding::LittleEndian::HostSwap16(kSuperFrameSpec)) && (mGtsSpec == 0) &&
               (mPendingAddressSpec == 0);
    }

    uint8_t *GetPayload(void) { return reinterpret_cast<uint8_t *>(this) + sizeof(*this); }

private:
    uint16_t mSuperframeSpec;
    uint8_t  mGtsSpec;
    uint8_t  mPendingAddressSpec;
} __attribute__((packed));

class BeaconPayload
{
public:
    enum
    {
        kProtocolId     = 3,  // Thread Protocol ID.
        kInfoStringSize = 92, // Max chars for the info string (@sa ToInfoString()).
    };

    enum
    {
        kProtocolVersion = 2,                     // Thread Protocol version.
        kVersionOffset   = 4,                     // Version field bit offset.
        kVersionMask     = 0xf << kVersionOffset, // Version field mask.
        kNativeFlag      = 1 << 3,                // Native Commissioner flag.
        kJoiningFlag     = 1 << 0,                // Joining Permitted flag.
    };

} __attribute__((packed));

} // namespace Mac
} // namespace Net
} // namespace vc

#endif /* NET_MAC_FRAME_HPP */
