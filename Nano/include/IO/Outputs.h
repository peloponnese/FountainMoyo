#ifndef OUTPUTS_H
#define OUTPUTS_H

#include <Arduino.h>

void outputsBegin();

void outputsSetPump(bool on);
void outputsSetValve1(bool on);
void outputsSetValve2(bool on);

void outputsSetPumpLed(bool on);
void outputsSetValve1Led(bool on);
void outputsSetValve2Led(bool on);
void outputsSetEmptyLed(bool on);

#endif