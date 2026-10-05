#ifndef WEB_SETTINGS_H
#define WEB_SETTINGS_H

#include <Arduino.h>

struct WebSettings
{
    struct
    {
        uint16_t dayPeriod;
        uint8_t dayRuntime;

        uint16_t nightPeriod;
        uint8_t nightRuntime;
    } pump;

    struct
    {
        uint8_t hour;
        uint8_t minute;

        uint8_t periodDays;
        uint8_t runtime;
    } watering;

    struct
    {
        bool enabled;
        bool loop;

        uint8_t hue;
        uint8_t saturation;
        uint8_t value;

        uint8_t rate;
    } leds;
};

extern WebSettings webSettings;

#endif