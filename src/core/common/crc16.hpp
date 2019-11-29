#ifndef CRC16_HPP
#define CRC16_HPP

#include "vcos-core-config.h"

#include <stdint.h>

namespace vc {

class Crc16
{
public:
    enum Polynomial
    {
        kCcitt = 0x1021, // CRC16_CCITT
        kAnsi  = 0x8005, // CRC16_ANSI
    };

    explicit Crc16(Polynomial aPolynomial);

    void Init(void) { mCrc = 0; }

    void Update(uint8_t aByte);

    uint16_t Get(void) const { return mCrc; }

private:
    uint16_t mPolynomial;
    uint16_t mCrc;
};

} // namespace vc

#endif /* CRC16_HPP */
