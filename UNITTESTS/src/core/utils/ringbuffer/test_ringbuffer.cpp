#include "gtest/gtest.h"
#include "utils/ringbuffer.hpp"

using namespace vc;

class TestRingBuffer : public testing::Test
{
protected:
    Utils::RingBuffer *obj;

    char buf[16];

    virtual void SetUp()
    {
        obj = new Utils::RingBuffer;
        obj->Init(buf, sizeof(buf));
    }

    virtual void TearDown()
    {
        delete obj;
    }
};

TEST_F(TestRingBuffer, constructor)
{
    EXPECT_TRUE(obj);

    EXPECT_EQ(obj->mBuf, buf);
    EXPECT_EQ(obj->mSize, 16);
    EXPECT_EQ(obj->mStart, 0);
    EXPECT_EQ(obj->mAvail, 0);
}

TEST_F(TestRingBuffer, initial_state)
{
    EXPECT_EQ(obj->Avail(), 0);
    EXPECT_EQ(obj->IsEmpty(), 1);
    EXPECT_EQ(obj->IsFull(), 0);
    EXPECT_EQ(obj->Free(), 16);
}

TEST_F(TestRingBuffer, add_peek_get_one_char)
{
    char dummy = '\n';

    obj->AddOne(dummy);

    EXPECT_EQ(obj->Avail(), 1);
    EXPECT_EQ(obj->IsEmpty(), 0);
    EXPECT_EQ(obj->IsFull(), 0);
    EXPECT_EQ(obj->Free(), 15);

    EXPECT_EQ(obj->PeekOne(), '\n');

    EXPECT_EQ(obj->Avail(), 1);
    EXPECT_EQ(obj->IsEmpty(), 0);
    EXPECT_EQ(obj->IsFull(), 0);
    EXPECT_EQ(obj->Free(), 15);

    EXPECT_EQ(obj->GetOne(), '\n');

    EXPECT_EQ(obj->Avail(), 0);
    EXPECT_EQ(obj->IsEmpty(), 1);
    EXPECT_EQ(obj->IsFull(), 0);
    EXPECT_EQ(obj->Free(), 16);
}

TEST_F(TestRingBuffer, add_peek_get_multiple_chars)
{
    char dummy[5] = {'d', 'a', 'r', 'k', 'o'};

    EXPECT_EQ(obj->Add(dummy, sizeof(dummy)), sizeof(dummy));

    EXPECT_EQ(obj->Avail(), 5);
    EXPECT_EQ(obj->IsEmpty(), 0);
    EXPECT_EQ(obj->IsFull(), 0);
    EXPECT_EQ(obj->Free(), 11);

    EXPECT_EQ(obj->GetOne(), 'd');

    EXPECT_EQ(obj->Avail(), 4);
    EXPECT_EQ(obj->IsEmpty(), 0);
    EXPECT_EQ(obj->IsFull(), 0);
    EXPECT_EQ(obj->Free(), 12);

    char dummypeek[5];

    EXPECT_EQ(obj->Peek(dummypeek, obj->Avail()), 4);

    EXPECT_EQ(obj->Avail(), 4);
    EXPECT_EQ(obj->IsEmpty(), 0);
    EXPECT_EQ(obj->IsFull(), 0);
    EXPECT_EQ(obj->Free(), 12);

    char dummyget[5];

    EXPECT_EQ(obj->Get(dummyget, obj->Avail()), 4);

    EXPECT_EQ(dummyget[0], dummypeek[0]);
    EXPECT_EQ(dummyget[1], dummypeek[1]);
    EXPECT_EQ(dummyget[2], dummypeek[2]);
    EXPECT_EQ(dummyget[3], dummypeek[3]);

    EXPECT_EQ(obj->Avail(), 0);
    EXPECT_EQ(obj->IsEmpty(), 1);
    EXPECT_EQ(obj->IsFull(), 0);
    EXPECT_EQ(obj->Free(), 16);
}

TEST_F(TestRingBuffer, maxout_remove_peek_chars)
{
    char dummy[16];

    for (unsigned i = 0; i < sizeof(dummy); i++)
    {
        dummy[i] = 'a';
    }

    EXPECT_EQ(obj->Add(dummy, sizeof(dummy)), sizeof(dummy));

    EXPECT_EQ(obj->Avail(), 16);
    EXPECT_EQ(obj->IsEmpty(), 0);
    EXPECT_EQ(obj->IsFull(), 1);
    EXPECT_EQ(obj->Free(), 0);

    obj->AddOne('b');

    EXPECT_EQ(obj->Avail(), 16);
    EXPECT_EQ(obj->IsEmpty(), 0);
    EXPECT_EQ(obj->IsFull(), 1);
    EXPECT_EQ(obj->Free(), 0);

    EXPECT_EQ(obj->Remove(5), 5);

    EXPECT_EQ(obj->Avail(), 11);
    EXPECT_EQ(obj->IsEmpty(), 0);
    EXPECT_EQ(obj->IsFull(), 0);
    EXPECT_EQ(obj->Free(), 5);

    obj->AddOne('c');

    EXPECT_EQ(obj->Avail(), 12);
    EXPECT_EQ(obj->IsEmpty(), 0);
    EXPECT_EQ(obj->IsFull(), 0);
    EXPECT_EQ(obj->Free(), 4);

    EXPECT_EQ(obj->Remove(10), 10);

    EXPECT_EQ(obj->Avail(), 2);
    EXPECT_EQ(obj->IsEmpty(), 0);
    EXPECT_EQ(obj->IsFull(), 0);
    EXPECT_EQ(obj->Free(), 14);

    char dummypeek[2];

    EXPECT_EQ(obj->Peek(dummypeek, sizeof(dummypeek)), 2);

    EXPECT_EQ(dummypeek[0], 'b');
    EXPECT_EQ(dummypeek[1], 'c');

    EXPECT_EQ(obj->Avail(), 2);
    EXPECT_EQ(obj->IsEmpty(), 0);
    EXPECT_EQ(obj->IsFull(), 0);
    EXPECT_EQ(obj->Free(), 14);

    char dummyget[2];

    EXPECT_EQ(obj->Get(dummyget, sizeof(dummyget)), 2);

    EXPECT_EQ(dummyget[0], dummypeek[0]);
    EXPECT_EQ(dummyget[1], dummypeek[1]);

    EXPECT_EQ(obj->Avail(), 0);
    EXPECT_EQ(obj->IsEmpty(), 1);
    EXPECT_EQ(obj->IsFull(), 0);
    EXPECT_EQ(obj->Free(), 16);
}
