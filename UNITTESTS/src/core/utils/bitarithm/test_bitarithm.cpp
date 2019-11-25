#include "gtest/gtest.h"
#include "utils/bitarithm.h"

class Bitarithm
{
public:
    Bitarithm()
    {
    }

    unsigned Msb(unsigned v)
    {
        return bitarithmMsb(v);
    }

    unsigned Lsb(unsigned v)
    {
        return bitarithmLsb(v);
    }

    unsigned BitsSet(unsigned v)
    {
        return bitarithmBitsSet(v);
    }
};

class TestBitarithm : public testing::Test
{
protected:
    Bitarithm *obj;

    virtual void SetUp()
    {
        obj = new Bitarithm;
    }

    virtual void TearDown()
    {
        delete obj;
    }
};

TEST_F(TestBitarithm, constructor)
{
    EXPECT_TRUE(obj);
}

TEST_F(TestBitarithm, msb_lsb_position)
{
    uint32_t bitcache = 0;

    bitcache |= (1 << 2);
    bitcache |= (1 << 3);
    bitcache |= (1 << 4);

    int lsb_pos = obj->Lsb(bitcache);
    int msb_pos = obj->Msb(bitcache);

    EXPECT_EQ(lsb_pos, 2);
    EXPECT_EQ(msb_pos, 4);

    bitcache |= (1 << 0);
    bitcache &= ~(1 << 4);

    lsb_pos = obj->Lsb(bitcache);
    msb_pos = obj->Msb(bitcache);

    EXPECT_EQ(lsb_pos, 0);
    EXPECT_EQ(msb_pos, 3);
}
