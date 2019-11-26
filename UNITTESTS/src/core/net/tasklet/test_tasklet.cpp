#include "gtest/gtest.h"

#include "common/instance.hpp"

#include "net/tasklet.hpp"

using namespace vc;

static uint8_t sHandlerCount = 0;
static uint8_t sTest1TaskletHandler = 0;
static uint8_t sTest2TaskletHandler = 0;
static uint8_t sTest3TaskletHandler = 0;
static uint8_t sTest4TaskletHandler = 0;

class TestTasklet : public testing::Test
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

TEST_F(TestTasklet, constructor)
{
    EXPECT_TRUE(obj);
    EXPECT_TRUE(obj->IsInitialized());
}

void Test1TaskletHandler(Net::Tasklet &aTasklet)
{
    sHandlerCount++;

    sTest1TaskletHandler = sHandlerCount;
}

void Test2TaskletHandler(Net::Tasklet &aTasklet)
{
    sHandlerCount++;

    sTest2TaskletHandler = sHandlerCount;
}

void Test3TaskletHandler(Net::Tasklet &aTasklet)
{
    sHandlerCount++;

    sTest3TaskletHandler = sHandlerCount;
}

void Test4TaskletHandler(Net::Tasklet &aTasklet)
{
    sHandlerCount++;

    sTest4TaskletHandler = sHandlerCount;
}

TEST_F(TestTasklet, functions)
{
    EXPECT_FALSE(obj->Get<Net::TaskletScheduler>().AreTaskletsPending());

    Net::Tasklet test1(Test1TaskletHandler);
    Net::Tasklet test2(Test2TaskletHandler);
    Net::Tasklet test3(Test3TaskletHandler);
    Net::Tasklet test4(Test4TaskletHandler);

    EXPECT_EQ(test1.Post(), VC_NET_ERROR_NONE);
    EXPECT_EQ(test2.Post(), VC_NET_ERROR_NONE);
    EXPECT_EQ(test3.Post(), VC_NET_ERROR_NONE);
    EXPECT_EQ(test4.Post(), VC_NET_ERROR_NONE);

    EXPECT_TRUE(obj->Get<Net::TaskletScheduler>().AreTaskletsPending());

    obj->Get<Net::TaskletScheduler>().ProcessQueuedTasklets();

    EXPECT_FALSE(obj->Get<Net::TaskletScheduler>().AreTaskletsPending());

    EXPECT_EQ(sTest1TaskletHandler, 1);
    EXPECT_EQ(sTest2TaskletHandler, 2);
    EXPECT_EQ(sTest3TaskletHandler, 3);
    EXPECT_EQ(sTest4TaskletHandler, 4);

    EXPECT_EQ(test4.Post(), VC_NET_ERROR_NONE);
    EXPECT_EQ(test3.Post(), VC_NET_ERROR_NONE);
    EXPECT_EQ(test2.Post(), VC_NET_ERROR_NONE);

    obj->Get<Net::TaskletScheduler>().ProcessQueuedTasklets();

    EXPECT_FALSE(obj->Get<Net::TaskletScheduler>().AreTaskletsPending());

    EXPECT_EQ(sTest1TaskletHandler, 1);
    EXPECT_EQ(sTest2TaskletHandler, 7);
    EXPECT_EQ(sTest3TaskletHandler, 6);
    EXPECT_EQ(sTest4TaskletHandler, 5);
}
