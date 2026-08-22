#ifndef PUMP_H
#define PUMP_H

#include <Arduino.h>
#include "Types.h"

void pumpBegin();
void pumpUpdate();

State pumpGetState();

#endif