#include "gtest/gtest.h"
#include "utils/isrpipe.hpp"

using namespace vc;

class TestIsripipeTsrb : public testing::Test
{
protected:
    Utils::Tsrb *obj;

    char buf[16];

    virtual void SetUp()
    {
        obj = new Utils::Tsrb(buf, sizeof(buf));
    }

    virtual void TearDown()
    {
        delete obj;
    }
};

TEST_F(TestIsripipeTsrb, constructor)
{
    EXPECT_TRUE(obj);
}

TEST_F(TestIsripipeTsrb, initial_state)
{
    EXPECT_EQ(obj->Avail(), 0);
    EXPECT_EQ(obj->IsEmpty(), true);
    EXPECT_EQ(obj->IsFull(), false);
    EXPECT_EQ(obj->Free(), 16);
}

TEST_F(TestIsripipeTsrb, add_get_one_char)
{
    char dummy = '\n';

    EXPECT_EQ(obj->AddOne(dummy), 0);

    EXPECT_EQ(obj->Avail(), 1);
    EXPECT_EQ(obj->IsEmpty(), false);
    EXPECT_EQ(obj->IsFull(), false);
    EXPECT_EQ(obj->Free(), 15);

    EXPECT_EQ(obj->GetOne(), dummy);

    EXPECT_EQ(obj->Avail(), 0);
    EXPECT_EQ(obj->IsEmpty(), true);
    EXPECT_EQ(obj->IsFull(), false);
    EXPECT_EQ(obj->Free(), 16);
}

TEST_F(TestIsripipeTsrb, add_get_multiple_chars)
{
    char dummy[5] = {'d', 'a', 'r', 'k', 'o'};

    EXPECT_EQ(obj->Add(static_cast<const char *>(dummy), sizeof(dummy)), sizeof(dummy));

    EXPECT_EQ(obj->Avail(), 5);
    EXPECT_EQ(obj->IsEmpty(), false);
    EXPECT_EQ(obj->IsFull(), false);
    EXPECT_EQ(obj->Free(), 11);

    EXPECT_EQ(obj->GetOne(), 'd');

    EXPECT_EQ(obj->Avail(), 4);
    EXPECT_EQ(obj->IsEmpty(), false);
    EXPECT_EQ(obj->IsFull(), false);
    EXPECT_EQ(obj->Free(), 12);

    char dummyget[5];

    EXPECT_EQ(obj->Get(dummyget, obj->Avail()), 4);

    EXPECT_EQ(dummyget[0], 'a');
    EXPECT_EQ(dummyget[1], 'r');
    EXPECT_EQ(dummyget[2], 'k');
    EXPECT_EQ(dummyget[3], 'o');

    EXPECT_EQ(obj->Avail(), 0);
    EXPECT_EQ(obj->IsEmpty(), true);
    EXPECT_EQ(obj->IsFull(), false);
    EXPECT_EQ(obj->Free(), 16);
}

TEST_F(TestIsripipeTsrb, drop_multiple_chars)
{
    char dummy[5] = {'d', 'a', 'r', 'k', 'o'};

    EXPECT_EQ(obj->Add(static_cast<const char *>(dummy), sizeof(dummy)), sizeof(dummy));

    EXPECT_EQ(obj->Avail(), 5);
    EXPECT_EQ(obj->IsEmpty(), false);
    EXPECT_EQ(obj->IsFull(), false);
    EXPECT_EQ(obj->Free(), 11);

    EXPECT_EQ(obj->Drop(3), 3);

    EXPECT_EQ(obj->Avail(), 2);
    EXPECT_EQ(obj->IsEmpty(), false);
    EXPECT_EQ(obj->IsFull(), false);
    EXPECT_EQ(obj->Free(), 14);

    char dummyget[5];

    EXPECT_EQ(obj->Get(dummyget, obj->Avail()), 2);

    EXPECT_EQ(dummyget[0], 'k');
    EXPECT_EQ(dummyget[1], 'o');

    EXPECT_EQ(obj->Avail(), 0);
    EXPECT_EQ(obj->IsEmpty(), true);
    EXPECT_EQ(obj->IsFull(), false);
    EXPECT_EQ(obj->Free(), 16);
}

TEST_F(TestIsripipeTsrb, maximum_size)
{
    char dummy[16];

    for (unsigned i = 0; i < sizeof(dummy); i++)
    {
        dummy[i] = 'a';
    }

    EXPECT_EQ(obj->Add(dummy, sizeof(dummy)), sizeof(dummy));

    EXPECT_EQ(obj->Avail(), 16);
    EXPECT_EQ(obj->IsEmpty(), false);
    EXPECT_EQ(obj->IsFull(), true);
    EXPECT_EQ(obj->Free(), 0);

    EXPECT_EQ(obj->AddOne('b'), -1);

    EXPECT_EQ(obj->Avail(), 16);
    EXPECT_EQ(obj->IsEmpty(), false);
    EXPECT_EQ(obj->IsFull(), true);
    EXPECT_EQ(obj->Free(), 0);

    EXPECT_EQ(obj->Drop(1), 1);
    EXPECT_EQ(obj->AddOne('b'), 0);

    EXPECT_EQ(obj->Avail(), 16);
    EXPECT_EQ(obj->IsEmpty(), false);
    EXPECT_EQ(obj->IsFull(), true);
    EXPECT_EQ(obj->Free(), 0);

    EXPECT_EQ(obj->Drop(15), 15);

    EXPECT_EQ(obj->Avail(), 1);
    EXPECT_EQ(obj->IsEmpty(), false);
    EXPECT_EQ(obj->IsFull(), false);
    EXPECT_EQ(obj->Free(), 15);

    EXPECT_EQ(obj->GetOne(), 'b');

    EXPECT_EQ(obj->Avail(), 0);
    EXPECT_EQ(obj->IsEmpty(), true);
    EXPECT_EQ(obj->IsFull(), false);
    EXPECT_EQ(obj->Free(), 16);
}
