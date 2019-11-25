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

TEST_F(TestThreadScheduler, initial_state)
{
    EXPECT_EQ(obj->GetSchedNumThreads(), 0);
    EXPECT_EQ(obj->GetContextSwitchRequest(), 0);
    EXPECT_EQ(obj->GetSchedActivePid(), KERNEL_PID_UNDEF);
    EXPECT_EQ(obj->GetRunqueueBitCache(), 0);
}
