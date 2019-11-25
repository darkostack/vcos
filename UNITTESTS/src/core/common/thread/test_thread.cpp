#include "gtest/gtest.h"
#include "common/thread.hpp"

using namespace vc;

class TestThreadScheduler : public testing::Test
{
protected:
    ThreadScheduler *obj;

    virtual void SetUp()
    {
        obj = new ThreadScheduler;
    }

    virtual void TearDown()
    {
        delete obj;
    }
};

TEST_F(TestThreadScheduler, constructor)
{
    EXPECT_TRUE(obj);
}
