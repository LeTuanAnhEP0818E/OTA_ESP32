#pragma once
#include <Arduino.h>

class PeriodicTimer
{
    using VoidCallBack = void (*)();

public:
    PeriodicTimer(VoidCallBack callbackFn)
        : callbackFn(callbackFn)
    {
        time_ms = millis();
    }

    void loop(unsigned int interval) // in seconds
    {
        if ((millis() - time_ms) >= (interval * 1000))
        {
            callbackFn();
            time_ms = millis();
        }
    }

private:
    VoidCallBack callbackFn{[]()
                            { return; }};
    unsigned long time_ms{0};
};