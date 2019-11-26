#ifndef NET_UTILS_HEAP_HPP
#define NET_UTILS_HEAP_HPP

#include "vcos-core-config.h"

#include <stddef.h>
#include <stdint.h>

#include "utils/static_assert.hpp"

namespace vc {
namespace Net {
namespace Utils {

class Block
{
    friend class Heap;

public:
    uint16_t GetSize(void) const { return mSize; }

    void SetSize(uint16_t aSize) { mSize = aSize; }

    uint16_t GetNext(void) const
    {
        return *reinterpret_cast<const uint16_t *>(reinterpret_cast<const uint8_t *>(this) + sizeof(mSize) + mSize);
    }

    void SetNext(uint16_t aNext)
    {
        *reinterpret_cast<uint16_t *>(reinterpret_cast<uint8_t *>(this) + sizeof(mSize) + mSize) = aNext;
    }

    void *GetPointer(void) { return &mMemory; }

    uint16_t GetLeftNext(void) const { return *(&mSize - 1); }

    bool IsLeftFree(void) const { return GetLeftNext() != 0; }

    bool IsFree(void) const { return mSize != kGuardBlockSize && GetNext() != 0; }

private:
    enum
    {
        kGuardBlockSize = 0xffff,
    };

    uint16_t mSize;

    uint8_t mMemory[sizeof(uint16_t)];
};

class Heap
{
public:
    Heap(void);

    void *CAlloc(size_t aCount, size_t aSize);

    void Free(void *aPointer);

    bool IsClean(void) const
    {
        Heap &       self  = *const_cast<Heap *>(this);
        const Block &super = self.BlockSuper();
        const Block &first = self.BlockRight(super);
        return super.GetNext() == self.BlockOffset(first) && first.GetSize() == kFirstBlockSize;
    }

    size_t GetCapacity(void) const { return kFirstBlockSize; }

    size_t GetFreeSize(void) const { return mMemory.mFreeSize; }

private:
    enum
    {
        kMemorySize         = VCOS_NET_CONFIG_HEAP_INTERNAL_SIZE,
        kAlignSize          = sizeof(void *),
        kBlockRemainderSize = kAlignSize - sizeof(uint16_t) * 2,
        kSuperBlockSize     = kAlignSize - sizeof(Block),
        kFirstBlockSize     = kMemorySize - kAlignSize * 3 + kBlockRemainderSize,
        kSuperBlockOffset   = kAlignSize - sizeof(uint16_t),
        kFirstBlockOffset   = kAlignSize * 2 - sizeof(uint16_t),
        kGuardBlockOffset   = kMemorySize - sizeof(uint16_t),
    };

    VC_STATIC_ASSERT(kMemorySize % kAlignSize == 0, "The heap memory size is not aligned to kAlignSize!");

    Block &BlockAt(uint16_t aOffset) { return *reinterpret_cast<Block *>(&mMemory.m16[aOffset / 2]); }

    Block &BlockOf(void *aPointer)
    {
        uint16_t offset = static_cast<uint16_t>(reinterpret_cast<uint8_t *>(aPointer) - mMemory.m8);
        offset -= sizeof(uint16_t);
        return BlockAt(offset);
    }

    Block &BlockSuper(void) { return BlockAt(kSuperBlockOffset); }

    Block &BlockNext(const Block &aBlock) { return BlockAt(aBlock.GetNext()); }

    Block &BlockRight(const Block &aBlock) { return BlockAt(BlockOffset(aBlock) + sizeof(Block) + aBlock.GetSize()); }

    Block &BlockPrev(const Block &aBlock);

    bool IsLeftFree(const Block &aBlock) { return (BlockOffset(aBlock) != kFirstBlockOffset && aBlock.IsLeftFree()); }

    uint16_t BlockOffset(const Block &aBlock)
    {
        return static_cast<uint16_t>(reinterpret_cast<const uint8_t *>(&aBlock) - mMemory.m8);
    }

    void BlockInsert(Block &aPrev, Block &aBlock);

    union
    {
        uint16_t mFreeSize;
        // Make sure memory is long aligned.
        long     mLong[kMemorySize / sizeof(long)];
        uint8_t  m8[kMemorySize];
        uint16_t m16[kMemorySize / sizeof(uint16_t)];
    } mMemory;
};

} // namespace Utils
} // namespace Net
} // namespace vc

#endif /* NET_UTILS_HEAP_HPP */
