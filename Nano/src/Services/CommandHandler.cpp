#include "Services/CommandHandler.h"

#include <string.h>
#include <ArduinoJson.h>

#include "Services/Comms.h"
#include "Services/Settings.h"

#include "Sensors/Environment.h"
#include "Control/Pump.h"
#include "Control/Watering.h"
#include "Control/Refill.h"


static void sendEnvironment(uint16_t id);
static void sendStatus(uint16_t id);
static void sendSettings(uint16_t id);

// --------------------------------------------------
// Begin
// --------------------------------------------------
void commandHandlerBegin()
{
}

// --------------------------------------------------
// Update
// --------------------------------------------------
void commandHandlerUpdate()
{
    if (!commsHasMessage())
        return;

    const char* command = commsGetCommand();

    const uint16_t id = commsGetId();

    // --------------------------------------------------
    // GET ENVIRONMENT
    // --------------------------------------------------
    if (strcmp(command, "get_environment") == 0)
    {
        sendEnvironment(id);
    }

    // --------------------------------------------------
    // GET STATUS
    // --------------------------------------------------
    else if (strcmp(command, "get_status") == 0)
    {
        sendStatus(id);
    }

    // --------------------------------------------------
    // GET SETTINGS
    // --------------------------------------------------
    else if (strcmp(command, "get_settings") == 0)
    {
        sendSettings(id);
    }

    // --------------------------------------------------
    // UNKNOWN COMMAND
    // --------------------------------------------------
    else
    {
        commsSendUnknownCommand(id);
    }

    commsClearMessage();
}

void sendEnvironment(uint16_t id)
{
    JsonDocument document;

    document["id"] = id;
    document["cmd"] = "environment";
    document["valid"] = environmentValid();

    if (environmentValid())
    {
        document["temperature"] = environmentTemperature();
        document["humidity"] = environmentHumidity();
    }

    commsSendJson(document);
    commsDebugJson(document);
}

void sendStatus(uint16_t id)
{
    JsonDocument document;

    document["id"] = id;
    document["cmd"] = "status";

    document["pump"] = pumpGetState();
    document["watering"] = wateringGetState();
    document["refill"] = refillGetState();
    document["empty"] = refillIsEmpty();

    commsSendJson(document);
}

void sendSettings(uint16_t id)
{
    JsonDocument document;

    document["id"] = id;
    document["cmd"] = "settings";

    document["pumpDayPeriod"] =
        settings.pump.dayPeriod;

    document["pumpDayRuntime"] =
        settings.pump.dayRunTime;

    document["pumpNightPeriod"] =
        settings.pump.nightPeriod;

    document["pumpNightRuntime"] =
        settings.pump.nightRunTime;

    document["waterHour"] =
        settings.water.hour;

    document["waterMinute"] =
        settings.water.minute;

    document["waterPeriodDays"] =
        settings.water.periodDays;

    document["waterRuntime"] =
        settings.water.runTime;

    document["ledEnabled"] =
        settings.led.enabled;

    document["ledBrightness"] =
        settings.led.brightness;

    document["ledRate"] =
        settings.led.rate;

    commsSendJson(document);
}