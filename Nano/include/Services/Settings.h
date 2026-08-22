#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

struct PumpSettings
{
    uint16_t dayPeriod; // minutes
    byte     dayRunTime; // minutes

    uint16_t nightPeriod; // minutes
    byte     nightRunTime; // minutes
};

struct WaterSettings
{
    byte hour; // hour of day
    byte minute; // minute of hour

    byte periodDays; // days
    byte runTime; // minutes
};

struct LedSettings
{
    bool enabled;
    byte brightness;
    byte rate;
};


struct Settings
{
    PumpSettings pump;
    WaterSettings water;
    LedSettings led;
};

extern Settings settings;

void settingsBegin();
void settingsValidate();

#endif