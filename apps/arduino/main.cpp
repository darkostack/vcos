#include <arduino/base.hpp>
#include <arduino/serialport.hpp>

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Hello Arduino!");
}

void loop(void)
{
    while (Serial.available())
    {
        char data = Serial.read();

        if (data == '\r' || data == '\n')
        {
            Serial.write("\r\n");
        }
        else
        {
            Serial.write(data);
        }
    }

    delay(10);
}
