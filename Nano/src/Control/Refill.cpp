#include "Control/Refill.h"

#include "IO/Inputs.h"
#include "IO/Outputs.h"
#include "Services/Scheduler.h"
#include "Control/Watering.h"

static State state = OFF;

static bool emptyFountain = false;
static byte waitSeconds = 0;

static void refillSetState(State newState)
{
    state = newState;

    switch(state)
    {
        case OFF:

            outputsSetValve2(false);
            outputsSetValve2Led(false);
            outputsSetEmptyLed(false);

            break;

        case WAITING:

            outputsSetValve2(false);
            outputsSetValve2Led(false);
            outputsSetEmptyLed(true);

            break;

        case RUNNING:

            outputsSetValve2(true);
            outputsSetValve2Led(true);
            outputsSetEmptyLed(true);

            break;

        case MANUAL:

            outputsSetValve2(true);
            outputsSetValve2Led(true);
            outputsSetEmptyLed(true);

            break;
    }
}

void refillBegin()
{
    emptyFountain = false;
    waitSeconds = 0;
    refillSetState(OFF);
}

State refillGetState()
{
    return state;
}

bool refillIsEmpty()
{
    return emptyFountain;
}

void refillUpdate()
{
    if (!schedulerSecondTick())
        return;

    const State wateringState = wateringGetState();

    switch(state)
    {

        case OFF:

            if (inputsReedLow() && inputsReedHigh())
            {
                emptyFountain = true;
                waitSeconds = 0;
                refillSetState(WAITING);
            }

            break;

        case WAITING:

            if (!inputsReedHigh())
            {
                emptyFountain = false;
                refillSetState(OFF);

                break;
            }

            if (wateringState == RUNNING || wateringState == MANUAL)
            {
                waitSeconds++;
                if (waitSeconds >= 10)
                {
                    refillSetState(RUNNING);
                }
            }
            else
            {
                waitSeconds = 0;
                refillSetState(OFF);
            }

            break;
        
        case RUNNING:

            if (!inputsReedHigh())
            {
                emptyFountain = false;
                refillSetState(OFF);
            }

            break;
        
        case MANUAL:
            break;
    }
}