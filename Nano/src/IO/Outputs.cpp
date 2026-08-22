#include "IO/Outputs.h"

#include "Pins.h"

void outputsBegin()
{
    pinMode(PIN_PUMP, OUTPUT);
    pinMode(PIN_VALVE1, OUTPUT);
    pinMode(PIN_VALVE2, OUTPUT);

    pinMode(PIN_LED_PUMP, OUTPUT);
    pinMode(PIN_LED_VALVE1, OUTPUT);
    pinMode(PIN_LED_VALVE2, OUTPUT);
    pinMode(PIN_LED_EMPTY, OUTPUT);

    outputsSetPump(false);
    outputsSetValve1(false);
    outputsSetValve2(false);

    outputsSetPumpLed(false);
    outputsSetValve1Led(false);
    outputsSetValve2Led(false);
    outputsSetEmptyLed(false);
}

void outputsSetPump(bool on)
{
    digitalWrite(PIN_PUMP, on);
}

void outputsSetValve1(bool on)
{
    digitalWrite(PIN_VALVE1, on);
}

void outputsSetValve2(bool on)
{
    digitalWrite(PIN_VALVE2, on);
}

void outputsSetPumpLed(bool on)
{
    digitalWrite(PIN_LED_PUMP, on);
}

void outputsSetValve1Led(bool on)
{
    digitalWrite(PIN_LED_VALVE1, on);
}

void outputsSetValve2Led(bool on)
{
    digitalWrite(PIN_LED_VALVE2, on);
}

void outputsSetEmptyLed(bool on)
{
    digitalWrite(PIN_LED_EMPTY, on);
}