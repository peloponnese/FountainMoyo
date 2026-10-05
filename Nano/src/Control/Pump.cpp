#include "Control/Pump.h"

#include "Pins.h"
#include "IO/Inputs.h"
#include "IO/Outputs.h"
#include "Services/Settings.h"
#include "Services/Scheduler.h"
#include "Sensors/DayNight.h"

static State state;

static uint16_t periodCounter;
static byte runCounter;

static void pumpSetState(State newState)
{
    state = newState;

    switch (state)
    {
        case OFF:
        case WAITING:

            outputsSetPump(false);
            outputsSetPumpLed(false);

            break;

        case RUNNING:
        case MANUAL:

            outputsSetPump(true);
            outputsSetPumpLed(true);

            break;
    }
}

void pumpBegin()
{
    periodCounter = 0;
    runCounter = 0;

    pumpSetState(OFF);
}

State pumpGetState()
{
    return state;
}

void pumpSetRunningExternal()
{
    periodCounter = 0;
    runCounter = 0;

    pumpSetState(RUNNING);
}

void pumpSetOffExternal()
{
    if (getDayState() == DAY)
        periodCounter = settings.pump.dayRunTime;
    else
        periodCounter = settings.pump.nightRunTime;
    
    runCounter = 0;

    pumpSetState(OFF);
}

void pumpUpdate()
{   
    //--------------------------------------------------
    // Manual mode
    //--------------------------------------------------

    if (inputsPumpSwitch() == LOW)
    {
        if (state != MANUAL)
            pumpSetState(MANUAL);

        return;
    }

    //--------------------------------------------------
    // Leave manual mode
    //--------------------------------------------------

    if (state == MANUAL)
    {
        periodCounter = 0;
        runCounter = 0;

        pumpSetState(OFF);
    }

    //--------------------------------------------------
    // Automatic mode
    //--------------------------------------------------
    
    if (!schedulerMinuteTick())
        return;

    periodCounter++;

    uint16_t period;
    byte runTime;

    if (getDayState() == DAY)
    {
        period = settings.pump.dayPeriod;
        runTime = settings.pump.dayRunTime;
    }
    else
    {
        period = settings.pump.nightPeriod;
        runTime = settings.pump.nightRunTime;
    }

    switch (state)
    {
        case OFF:

            if (periodCounter >= period)
            {
                periodCounter = 0;
                runCounter = 0;

                pumpSetState(RUNNING);
            }

            break;

        case RUNNING:

            runCounter++;

            if (runCounter >= runTime)
            {
                pumpSetState(OFF);
            }

            break;

        default:
            break;
    }
}