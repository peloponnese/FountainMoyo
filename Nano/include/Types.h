#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

enum State : byte
{
    OFF = 0,
    WAITING,
    RUNNING,
    MANUAL
};

enum DayState : bool
{
    NIGHT = false,
    DAY = true
};

#endif