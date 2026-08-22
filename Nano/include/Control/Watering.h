#ifndef WATERING_H
#define WATERING_H

#include <Arduino.h>
#include "Types.h"

void wateringBegin();
void wateringUpdate();

State wateringGetState();

#endif