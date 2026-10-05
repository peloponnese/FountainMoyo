#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <Arduino.h>

void ledStripBegin();
void ledStripUpdate();
void ledStripOutput();

uint8_t getLedStripHue();

#endif