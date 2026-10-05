#ifndef WEB_STATE_H
#define WEB_STATE_H

#include <Arduino.h>

struct WebState
{
    struct 
    {
        bool pump;
        bool watering;
        bool refill;
        bool empty;
        bool dayNight;
    } control;

    struct
    {
        float temperature;
        float humidity;
    } environment;
};

extern WebState webState;

#endif