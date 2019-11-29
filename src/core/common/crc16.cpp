#include "common/crc16.hpp"

namespace vc {

Crc16::Crc16(Polynomial aPolynomial)
{
    mPolynomial = static_cast<uint16_t>(aPolynomial);
    Init();
}

void Crc16::Update(uint8_t aByte)
{
    uint8_t i;

    mCrc = mCrc ^ static_cast<uint16_t>(aByte << 8);
    i    = 8;

    do
    {
        if (mCrc & 0x8000)
        {
            mCrc = static_cast<uint16_t>(mCrc << 1) ^ mPolynomial;
        }
        else
        {
            mCrc = static_cast<uint16_t>(mCrc << 1);
        }
    } while (--i);
}

} // namespace vc
