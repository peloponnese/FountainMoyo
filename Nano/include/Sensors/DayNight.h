#ifndef DAYNIGHT_H
#define DAYNIGHT_H

#include <Arduino.h>
#include "Types.h"

struct DayNightSettings
{
    uint16_t dayThreshold;
    uint16_t nightThreshold;
};

void dayNightBegin();
void dayNightUpdate();

DayState getDayState();

#endif