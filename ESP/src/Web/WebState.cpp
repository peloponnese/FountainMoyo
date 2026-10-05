#include "Web/WebState.h"

WebState webState =
{
    {
        false,  // pump
        false,  // watering
        false,  // refill
        false,  // empty
        false   // dayNight
    },

    {
        0.0,    // temperature
        0.0    // humidity
    }
};