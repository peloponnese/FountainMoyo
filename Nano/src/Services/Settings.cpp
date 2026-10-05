#include "Services/Settings.h"

#include "Config.h"

Settings settings;

void settingsBegin()
{
    settings.pump.dayPeriod = 30; // minutes
    settings.pump.dayRunTime = 10; // minutes

    settings.pump.nightPeriod = 120; // minutes
    settings.pump.nightRunTime = 10; // minutes

    settings.water.hour = 19; // hour
    settings.water.minute = 30; // minutes

    settings.water.periodDays = 2; // days
    settings.water.runTime = 10; // minutes

    settings.leds.enabled = true;
    settings.leds.loopMode = true;
    settings.leds.rate = 10;
    settings.leds.hue = 255;
    settings.leds.saturation = 255;
    settings.leds.value = 255;

    settingsValidate();
}

void settingsValidate()
{
    settings.pump.dayPeriod =
        constrain(settings.pump.dayPeriod,
                  PUMP_PERIOD_MIN,
                  PUMP_PERIOD_MAX);

    settings.pump.dayRunTime =
        constrain(settings.pump.dayRunTime,
                  PUMP_RUNTIME_MIN,
                  PUMP_RUNTIME_MAX);

    settings.pump.nightPeriod =
        constrain(settings.pump.nightPeriod,
                  PUMP_PERIOD_MIN,
                  PUMP_PERIOD_MAX);

    settings.pump.nightRunTime =
        constrain(settings.pump.nightRunTime,
                  PUMP_RUNTIME_MIN,
                  PUMP_RUNTIME_MAX);

    settings.water.hour =
        constrain(settings.water.hour,
                  WATER_HOUR_MIN,
                  WATER_HOUR_MAX);

    settings.water.minute =
        constrain(settings.water.minute,
                  WATER_MINUTE_MIN,
                  WATER_MINUTE_MAX);

    // Force 15-minute resolution
    settings.water.minute =
        (settings.water.minute / WATER_MINUTE_STEP) *
        WATER_MINUTE_STEP;

    settings.water.periodDays =
        constrain(settings.water.periodDays,
                  WATER_PERIOD_MIN,
                  WATER_PERIOD_MAX);

    settings.water.runTime =
        constrain(settings.water.runTime,
                  WATER_RUNTIME_MIN,
                  WATER_RUNTIME_MAX);

    settings.leds.rate =
        constrain(settings.leds.rate,
                  LED_RATE_MIN_MS,
                  LED_RATE_MAX_MS);
    
    settings.leds.hue =
        constrain(settings.leds.hue,
                  0,
                  255);

    settings.leds.saturation =
        constrain(settings.leds.saturation,
                  0,
                  255);

    settings.leds.value =
        constrain(settings.leds.value,
                  0,
                  255);
}