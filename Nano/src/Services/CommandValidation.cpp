#include "Services/CommandValidation.h"

bool validatePumpSettings(
    JsonDocument& message,
    uint16_t& dayPeriod,
    uint8_t& dayRuntime,
    uint16_t& nightPeriod,
    uint8_t& nightRuntime)
{
    if (!message["dayPeriod"].is<int>() ||
        !message["dayRuntime"].is<int>() ||
        !message["nightPeriod"].is<int>() ||
        !message["nightRuntime"].is<int>())
    {
        return false;
    }

    dayPeriod = message["dayPeriod"].as<int>();
    dayRuntime = message["dayRuntime"].as<int>();
    nightPeriod = message["nightPeriod"].as<int>();
    nightRuntime = message["nightRuntime"].as<int>();

    return true;
}

bool validateWateringSettings(
    JsonDocument& message,
    uint8_t& hour,
    uint8_t& minute,
    uint8_t& periodDays,
    uint8_t& runtime)
{
    if (!message["hour"].is<int>() ||
        !message["minute"].is<int>() ||
        !message["periodDays"].is<int>() ||
        !message["runtime"].is<int>())
    {
        return false;
    }

    hour = message["hour"].as<int>();
    minute = message["minute"].as<int>();
    periodDays = message["periodDays"].as<int>();
    runtime = message["runtime"].as<int>();

    return true;
}

bool validateLedsSettings(
    JsonDocument& message,
    bool& enabled,
    bool& loop,
    uint8_t& rate,
    uint8_t& hue,
    uint8_t& saturation,
    uint8_t& value)
{
    if (!message["enabled"].is<bool>() ||
        !message["loop"].is<bool>() ||
        !message["rate"].is<int>() ||
        !message["H"].is<int>() ||
        !message["S"].is<int>() ||
        !message["V"].is<int>())
    {
        return false;
    }

    enabled = message["enabled"].as<bool>();
    loop = message["loop"].as<bool>();
    rate = message["rate"].as<int>();
    hue = message["H"].as<int>();
    saturation = message["S"].as<int>();
    value = message["V"].as<int>();

    return true;
}

bool validateSetPump(
    JsonDocument& message,
    bool& manualOperation
)
{
    if (!message["enabled"].is<bool>())
        return false;

    manualOperation = message["enabled"].as<bool>();

    return true;
}

bool validateSetWatering(
    JsonDocument& message,
    bool& manualOperation
)
{
    if (!message["enabled"].is<bool>())
        return false;

    manualOperation = message["enabled"].as<bool>();

    return true;
}

bool validateSetLeds(
    JsonDocument& message,
    bool& loop,
    uint8_t& hue
)
{
    if (!message["loop"].is<bool>() ||
        !message["hue"].is<int>())
    {
        return false;
    }

    loop = message["loop"].as<bool>();
    hue = message["hue"].as<int>();

    return true;
}