#include "gtest/gtest.h"
#include "common/list.hpp"

using namespace vc;

class TestList : public testing::Test
{
protected:
    List *obj;

    virtual void SetUp()
    {
        obj = new List;
    }

    virtual void TearDown()
    {
        delete obj;
    }
};

TEST_F(TestList, constructor)
{
    EXPECT_TRUE(obj);
}

TEST_F(TestList, add_remove_head)
{
    List newList1;
    List newList2;

    newList1.mNext = NULL;
    newList2.mNext = NULL;

    obj->Add(&newList1);
    obj->Add(&newList2);

    EXPECT_EQ(obj->mNext, &newList2);
    EXPECT_EQ((obj->mNext)->mNext, &newList1);
    EXPECT_EQ(((obj->mNext)->mNext)->mNext, newList1.mNext);

    EXPECT_EQ(obj->RemoveHead(), &newList2);

    EXPECT_EQ(obj->mNext, &newList1);
    EXPECT_EQ((obj->mNext)->mNext, newList1.mNext);

    obj->Add(&newList2);

    EXPECT_EQ(obj->mNext, &newList2);
    EXPECT_EQ((obj->mNext)->mNext, &newList1);
    EXPECT_EQ(((obj->mNext)->mNext)->mNext, newList1.mNext);

    EXPECT_EQ(List::Remove(obj, &newList1), &newList1);

    EXPECT_EQ(obj->mNext, &newList2);
    EXPECT_EQ((obj->mNext)->mNext, newList1.mNext);
}
