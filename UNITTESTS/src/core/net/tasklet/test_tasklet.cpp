#include "gtest/gtest.h"

#include "common/instance.hpp"

#include "net/tasklet.hpp"

using namespace vc;

static uint8_t sCallbackCount = 0;

class TestNetTasklet : public testing::Test
{
protected:
    Instance *obj;

    virtual void SetUp()
    {
        obj = &Instance::Init();
    }

    virtual void TearDown()
    {
    }
};

class TestTasklet : public Net::Tasklet
{
public:
    TestTasklet(void)
        : Net::Tasklet(TestTasklet::HandleTaskletCallback)
        , mCallbackCount(0)
    {
    }

    static void HandleTaskletCallback(Net::Tasklet &aTasklet)
    {
        static_cast<TestTasklet &>(aTasklet).HandleTaskletCallback();
    }

    void HandleTaskletCallback(void)
    {
        sCallbackCount++;
        mCallbackCount = sCallbackCount;
    }

    uint8_t GetCallbackCount(void) { return mCallbackCount; }

    void ResetCallbackCount(void) { mCallbackCount = 0; }

private:
    uint8_t mCallbackCount;
};

TEST_F(TestNetTasklet, constructor)
{
    EXPECT_TRUE(obj);
    EXPECT_TRUE(obj->IsInitialized());
}

TEST_F(TestNetTasklet, functions)
{
    EXPECT_FALSE(obj->Get<Net::TaskletScheduler>().AreTaskletsPending());

    TestTasklet task1;
    TestTasklet task2;
    TestTasklet task3;
    TestTasklet task4;

    EXPECT_EQ(task1.Post(), VC_NET_ERROR_NONE);
    EXPECT_EQ(task2.Post(), VC_NET_ERROR_NONE);
    EXPECT_EQ(task3.Post(), VC_NET_ERROR_NONE);
    EXPECT_EQ(task4.Post(), VC_NET_ERROR_NONE);

    EXPECT_TRUE(obj->Get<Net::TaskletScheduler>().AreTaskletsPending());

    obj->Get<Net::TaskletScheduler>().ProcessQueuedTasklets();

    EXPECT_FALSE(obj->Get<Net::TaskletScheduler>().AreTaskletsPending());

    EXPECT_EQ(task1.GetCallbackCount(), 1);
    EXPECT_EQ(task2.GetCallbackCount(), 2);
    EXPECT_EQ(task3.GetCallbackCount(), 3);
    EXPECT_EQ(task4.GetCallbackCount(), 4);

    EXPECT_EQ(task4.Post(), VC_NET_ERROR_NONE);
    EXPECT_EQ(task3.Post(), VC_NET_ERROR_NONE);
    EXPECT_EQ(task2.Post(), VC_NET_ERROR_NONE);

    EXPECT_TRUE(obj->Get<Net::TaskletScheduler>().AreTaskletsPending());

    obj->Get<Net::TaskletScheduler>().ProcessQueuedTasklets();

    EXPECT_FALSE(obj->Get<Net::TaskletScheduler>().AreTaskletsPending());

    EXPECT_EQ(task1.GetCallbackCount(), 1);
    EXPECT_EQ(task2.GetCallbackCount(), 7);
    EXPECT_EQ(task3.GetCallbackCount(), 6);
    EXPECT_EQ(task4.GetCallbackCount(), 5);
}
