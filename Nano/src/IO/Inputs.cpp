#include "IO/Inputs.h"

#include "Pins.h"

static bool pumpSwitch  = false;
static bool waterSwitch = false;
static bool reedHigh    = false;
static bool reedLow     = false;

void inputsBegin()
{
    pinMode(PIN_SW_PUMP, INPUT_PULLUP);
    pinMode(PIN_SW_WATER, INPUT_PULLUP);

    pinMode(PIN_REED_HIGH, INPUT_PULLUP);
    pinMode(PIN_REED_LOW, INPUT_PULLUP);

    inputsUpdate();
}

void inputsUpdate()
{
    pumpSwitch  = (digitalRead(PIN_SW_PUMP) == LOW);
    waterSwitch = (digitalRead(PIN_SW_WATER) == LOW);
    reedHigh    = (digitalRead(PIN_REED_HIGH) == LOW);
    reedLow     = (digitalRead(PIN_REED_LOW) == LOW);
}

bool inputsPumpSwitch()
{
    return pumpSwitch;
}

bool inputsWaterSwitch()
{
    return waterSwitch;
}

bool inputsReedHigh()
{
    return reedHigh;
}

bool inputsReedLow()
{
    return reedLow;
}