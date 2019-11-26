#include "gtest/gtest.h"

#include "net/utils/heap.hpp"

#include <string.h>

using namespace vc;

class TestHeap : public testing::Test
{
protected:
    Net::Utils::Heap *obj;

    virtual void SetUp()
    {
        obj = new Net::Utils::Heap;
    }

    virtual void TearDown()
    {
        delete obj;
    }
};

TEST_F(TestHeap, constructor)
{
    EXPECT_TRUE(obj);
}

TEST_F(TestHeap, test_allocate_single)
{
    const size_t totalSize = obj->GetFreeSize();

    {
        void *p = obj->CAlloc(1, 0);

        EXPECT_FALSE(p);
        EXPECT_EQ(totalSize, obj->GetFreeSize());

        obj->Free(p);

        p = obj->CAlloc(0, 1);

        EXPECT_FALSE(p);
        EXPECT_EQ(totalSize, obj->GetFreeSize());
    }

    for (size_t size = 1; size <= obj->GetCapacity(); ++size)
    {
        void *p = obj->CAlloc(1, size);

        EXPECT_TRUE(p);
        EXPECT_TRUE(!obj->IsClean());
        EXPECT_TRUE(obj->GetFreeSize() + size <= totalSize);

        memset(p, 0xff, size);

        obj->Free(p);

        EXPECT_TRUE(obj->IsClean());
        EXPECT_TRUE(obj->GetFreeSize() == totalSize);
    }

    EXPECT_TRUE(obj->IsClean());
    EXPECT_TRUE(obj->GetFreeSize() == totalSize);
}

void TestAllocateRandomly(Net::Utils::Heap *aHeap, size_t aSizeLimit, unsigned int aSeed)
{
    struct Node
    {
        Node *mNext;
        size_t mSize;
    };

    Node head;
    size_t nnodes = 0;

    srand(aSeed);

    const size_t totalSize = aHeap->GetFreeSize();

    Node *last = &head;

    do
    {
        size_t size = sizeof(Node) + static_cast<size_t>(rand()) % aSizeLimit;

        last->mNext = static_cast<Node *>(aHeap->CAlloc(1, size));

        if (last->mNext == NULL)
        {
            // No more memory for allocation
            break;
        }

        EXPECT_TRUE(last->mNext->mNext == NULL);

        last = last->mNext;
        last->mSize = size;

        ++nnodes;

        // 50% probability to randomly free a node.
        size_t freeIndex = static_cast<size_t>(rand()) % (nnodes * 2);

        if (freeIndex > nnodes)
        {
            freeIndex /= 2;

            Node *prev = &head;

            while (freeIndex--)
            {
                prev = prev->mNext;
            }

            Node *curr = prev->mNext;
            prev->mNext = curr->mNext;

            aHeap->Free(curr);

            if (last == curr)
            {
                last = prev;
            }

            --nnodes;
        }
    } while (true);

    last = head.mNext;

    while (last)
    {
        Node *next = last->mNext;
        aHeap->Free(last);
        last = next;
    }

    EXPECT_TRUE(aHeap->IsClean());
    EXPECT_TRUE(aHeap->GetFreeSize() == totalSize);
}

TEST_F(TestHeap, test_allocate_multiple)
{
    for (unsigned int seed = 0; seed < 10; ++seed)
    {
        size_t sizeLimit = (1 << seed);
        TestAllocateRandomly(obj, sizeLimit, seed);
    }
}
