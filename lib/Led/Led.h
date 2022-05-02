#ifndef LED_H
#define LED_H

#include <Arduino.h>

class Led
{
public:
    Led(uint8_t pin);

    void init(bool _isNegated);
    void on(uint8_t power);
    void off();
    void signalizeError();
    void signalizeLowBattery();
    void dim(uint16_t timeMs);

private:
    uint8_t _pin;
    bool _isOn;
    bool _isNegated;
};

#endif