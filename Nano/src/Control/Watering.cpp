#include "Control/Watering.h"

#include "Pins.h"
#include "IO/Inputs.h"
#include "IO/Outputs.h"
#include "Services/Settings.h"
#include "Services/Scheduler.h"

static State state;

static byte runCounter;

// Prevents multiple starts during the same day
static uint16_t lastStartDay = 65535;

static void wateringSetState(State newState)
{
    state = newState;

    switch (state)
    {
        case OFF:
            outputsSetValve1(false);
            outputsSetValve1Led(false);

            break;

        case RUNNING:
        case MANUAL:

            outputsSetValve1(true);
            outputsSetValve1Led(true);

            break;

        default:
            break;
    }
}

void wateringBegin()
{
    runCounter = 0;

    wateringSetState(OFF);
}

State wateringGetState()
{
    return state;
}

void wateringSetRunningExternal()
{
    lastStartDay = dateTime.day;
    runCounter = 0;

    wateringSetState(RUNNING);
}

void wateringSetOffExternal()
{
    wateringSetState(OFF);
}

void wateringUpdate()
{
    //--------------------------------------------------
    // Manual mode
    //--------------------------------------------------

    if (inputsWaterSwitch() == LOW)
    {
        if (state != MANUAL)
            wateringSetState(MANUAL);

        return;
    }

    //--------------------------------------------------
    // Leave manual mode
    //--------------------------------------------------

    if (state == MANUAL)
    {
        runCounter = 0;
        wateringSetState(OFF);
    }

    //--------------------------------------------------
    // Automatic cycle
    //--------------------------------------------------

    if (!schedulerMinuteTick())
        return;

    switch (state)
    {
        case OFF:

            if (dateTime.day != lastStartDay &&
                (dateTime.day % settings.water.periodDays) == 0 &&
                dateTime.hour == settings.water.hour &&
                dateTime.minute == settings.water.minute)
            {
                lastStartDay = dateTime.day;
                runCounter = 0;

                wateringSetState(RUNNING);
            }

            break;

        case RUNNING:

            runCounter++;

            if (runCounter >= settings.water.runTime)
            {
                wateringSetState(OFF);
            }

            break;

        default:
            break;
    }
}