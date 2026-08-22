#include "Sensors/DayNight.h"

#include "Pins.h"
#include "Config.h"
#include "Services/Scheduler.h"

static DayState state;

void dayNightBegin()
{
    const uint16_t value = analogRead(PIN_LDR);

    if (value >= LDR_DAY_THRESHOLD)
        state = DAY;
    else
        state = NIGHT;
}

void dayNightUpdate()
{
    if (!schedulerSecondTick())
        return;

    const uint16_t value = analogRead(PIN_LDR);

    switch (state)
    {
        case DAY:

            if (value < LDR_NIGHT_THRESHOLD)
                state = NIGHT;

            break;

        case NIGHT:

            if (value > LDR_DAY_THRESHOLD)
                state = DAY;

            break;
    }
}

DayState getDayState()
{
    return state;
}