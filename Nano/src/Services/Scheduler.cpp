#include "Services/Scheduler.h"

DateTime dateTime;

static unsigned long previousMillis;

static bool secondTick;
static bool minuteTick;

void schedulerBegin()
{
    previousMillis = millis();

    secondTick = false;
    minuteTick = false;

    dateTime.second = 0;
    dateTime.minute = 0;
    dateTime.hour = 0;
    dateTime.day = 0;
}

void schedulerSetTime(
    byte hour,
    byte minute,
    byte second,
    uint16_t day)
{
    if (hour > 23) return;
    if (minute > 59) return;
    if (second > 59) return;

    dateTime.hour = hour;
    dateTime.minute = minute;
    dateTime.second = second;
    dateTime.day = day;

    previousMillis = millis();
}

void schedulerUpdate()
{
    secondTick = false;
    minuteTick = false;

    unsigned long now = millis();

    while (now - previousMillis >= 1000UL)
    {
        previousMillis += 1000UL;

        secondTick = true;

        dateTime.second++;

        if (dateTime.second >= 60)
        {
            dateTime.second = 0;
            dateTime.minute++;

            minuteTick = true;

            if (dateTime.minute >= 60)
            {
                dateTime.minute = 0;
                dateTime.hour++;

                if (dateTime.hour >= 24)
                {
                    dateTime.hour = 0;
                    dateTime.day++;
                }
            }
        }
    }
}

bool schedulerSecondTick()
{
    return secondTick;
}

bool schedulerMinuteTick()
{
    return minuteTick;
}