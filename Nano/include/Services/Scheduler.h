#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

struct DateTime
{
    byte second;
    byte minute;
    byte hour;
    uint16_t day;
};

extern DateTime dateTime;

void schedulerBegin();
void schedulerUpdate();

void schedulerSetTime(
    byte hour,
    byte minute,
    byte second,
    uint16_t day);

bool schedulerSecondTick();
bool schedulerMinuteTick();

#endif