#ifndef REFILL_H
#define REFILL_H

#include <Arduino.h>
#include "Types.h"

void refillBegin();
void refillUpdate();

State refillGetState();

bool refillIsEmpty();

#endif