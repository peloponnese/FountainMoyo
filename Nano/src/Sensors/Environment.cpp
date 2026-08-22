#include "Sensors/Environment.h"

#include <DHT.h>

#include "Config.h"
#include "Pins.h"
#include "Services/Scheduler.h"

struct EnvironmentData
{
    float temperature;
    float humidity;
    bool valid;
};

static DHT dht(PIN_DHT, DHT22);

static EnvironmentData data;

static byte seconds = 0;

void environmentBegin()
{
    dht.begin();

    randomSeed(millis());

    data.temperature = 0.0f;
    data.humidity = 0.0f;
    data.valid = false;

    seconds = ENVIRONMENT_PERIOD_S;
}

void environmentUpdate()
{
    if (!schedulerSecondTick())
        return;

    if (++seconds < ENVIRONMENT_PERIOD_S)
        return;

    seconds = 0;

    // const float temperature = dht.readTemperature();
    // const float humidity = dht.readHumidity();

    // if (isnan(temperature) || isnan(humidity))
    // {
    //     data.valid = false;
    //     return;
    // }
    const float temperature = random(150, 450) / 10.0f;

    const float humidity = random(200, 900) / 10.0f;

    data.temperature = temperature;
    data.humidity = humidity;
    data.valid = true;
}

bool environmentValid()
{
    return data.valid;
}

float environmentTemperature()
{
    return data.temperature;
}

float environmentHumidity()
{
    return data.humidity;
}