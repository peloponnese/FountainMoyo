#ifndef INPUTS_H
#define INPUTS_H

#include <Arduino.h>

void inputsBegin();
void inputsUpdate();

bool inputsPumpSwitch();
bool inputsWaterSwitch();

bool inputsReedHigh();
bool inputsReedLow();

#endif