#include "gtest/gtest.h"
#include "example/MyClass.h"

class TestMyClass : public testing::Test {
protected:
    example::MyClass *obj;

    virtual void SetUp()
    {
        obj = new example::MyClass();
    }

    virtual void TearDown()
    {
        delete obj;
    }
};

TEST_F(TestMyClass, constructor)
{
    EXPECT_TRUE(obj);
}

TEST_F(TestMyClass, myfunction)
{
    EXPECT_EQ(obj->myFunction(), 0);
}
