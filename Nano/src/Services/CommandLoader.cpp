#include "Services/CommandLoader.h"

#include <ArduinoJson.h>

#include "Services/Settings.h"

#include "Sensors/Environment.h"
#include "Sensors/DayNight.h"

#include "Control/Pump.h"
#include "Control/Watering.h"
#include "Control/Refill.h"

#include "IO/LEDStrip.h"

// --------------------------------------------------
// Pending reply
// --------------------------------------------------

static JsonDocument document;
static bool commandPending = false;

// --------------------------------------------------
// Internal helper
// --------------------------------------------------

static void prepareCommand(uint16_t id, const char* command)
{
    document.clear();

    document["id"] = id;
    document["cmd"] = command;

    commandPending = true;
}

// --------------------------------------------------
// GET_STATE: Environment
// --------------------------------------------------

void loadCommandEnvironment(uint16_t id)
{
    prepareCommand(id, "environment");

    document["valid"] = environmentValid();

    if (environmentValid())
    {
        document["temperature"] = environmentTemperature();
        document["humidity"] = environmentHumidity();
    }
}

// --------------------------------------------------
// GET_STATE: Control
// --------------------------------------------------

void loadCommandControl(uint16_t id)
{
    prepareCommand(id, "control");

    document["pump"] = pumpGetState() >= RUNNING;
    document["watering"] = wateringGetState() >= RUNNING;
    document["refill"] = refillGetState() >= RUNNING;
    document["empty"] = refillIsEmpty();
    document["dayNight"] = getDayState() == DAY;
}

// --------------------------------------------------
// GET_STATE: Lights
// --------------------------------------------------

void loadCommandLeds(uint16_t id)
{
    prepareCommand(id, "leds");

    document["loop"] = settings.leds.loopMode;
    document["H"] = getLedStripHue();
}

// --------------------------------------------------
// GET_SETTINGS: Pump
// --------------------------------------------------

void loadCommandSettingsPump(uint16_t id)
{
    prepareCommand(id, "pump");

    document["dayPeriod"] = settings.pump.dayPeriod;
    document["dayRuntime"] = settings.pump.dayRunTime;
    document["nightPeriod"] = settings.pump.nightPeriod;
    document["nightRuntime"] = settings.pump.nightRunTime;
}

// --------------------------------------------------
// GET_SETTINGS: Watering
// --------------------------------------------------

void loadCommandSettingsWatering(uint16_t id)
{
    prepareCommand(id, "watering");

    document["hour"] = settings.water.hour;
    document["minute"] = settings.water.minute;
    document["periodDays"] = settings.water.periodDays;
    document["runtime"] = settings.water.runTime;
}

// --------------------------------------------------
// GET_SETTINGS: Lights
// --------------------------------------------------

void loadCommandSettingsLeds(uint16_t id)
{
    prepareCommand(id, "leds");

    document["enabled"] = settings.leds.enabled;
    document["loop"] = settings.leds.loopMode;
    document["rate"] = settings.leds.rate;
    document["H"] = getLedStripHue();
    document["S"] = settings.leds.saturation;
    document["V"] = settings.leds.value;
}

// --------------------------------------------------
// ACK
// --------------------------------------------------

void loadCommandAck(uint16_t id)
{
    prepareCommand(id, "ack");
}

// --------------------------------------------------
// Pending reply
// --------------------------------------------------

bool commandHasReply()
{
    return commandPending;
}

JsonDocument& commandGetReply()
{
    return document;
}

void commandClearReply()
{
    document.clear();
    commandPending = false;
}