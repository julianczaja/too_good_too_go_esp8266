#include "Led.h"

Led::Led(uint8_t pin)
{
    _pin = pin;
}

void Led::init(bool negated)
{
    _isNegated = negated;
    pinMode(_pin, OUTPUT);
    off();
}

void Led::on(uint8_t power)
{
    if (power <= 0)
        power = 1;
    else if (power > 100)
        power = 100;

    int val = (power / 100.0) * 1023;

    if (_isNegated)
    {
        analogWrite(_pin, 1023 - val);
    }
    else
    {
        analogWrite(_pin, val);
    }
    _isOn = true;
}

void Led::off()
{
    if (_isNegated)
    {
        analogWrite(_pin, 1023);
    }
    else
    {
        analogWrite(_pin, 0);
    }
    _isOn = false;
}

void Led::signalizeError()
{
    off();
    delay(100);
    on(100);
    delay(100);
    off();
    delay(100);
    on(100);
    delay(100);
    off();
    delay(100);
    on(100);
    delay(100);
    off();
}

void Led::signalizeLowBattery()
{
    dim(100);
    delay(100);
    dim(100);
    delay(100);
    dim(100);
}

void Led::dim(uint16_t timeMs)
{
    int16_t delayTimeUs = ((timeMs / 1023.0) / 2.0) * 16 * 1000;
    int16_t i;

    if (_isNegated)
    {
        for (i = 1023; i > 0; i -= 16)
        {
            analogWrite(_pin, i);
            delayMicroseconds(delayTimeUs);
        }
        for (i = 0; i < 1023; i += 16)
        {
            analogWrite(_pin, i);
            delayMicroseconds(delayTimeUs);
        }
    }
    else
    {
        for (i = 0; i < 1023; i += 16)
        {
            analogWrite(_pin, i);
            delayMicroseconds(delayTimeUs);
        }
        for (i = 1023; i > 0; i -= 16)
        {
            analogWrite(_pin, i);
            delayMicroseconds(delayTimeUs);
        }
    }

    off();
}
