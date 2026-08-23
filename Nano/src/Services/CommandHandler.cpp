#include "Services/CommandHandler.h"

#include <string.h>
#include <ArduinoJson.h>

#include "Config.h"

#include "Services/Comms.h"
#include "Services/Settings.h"

#include "Sensors/Environment.h"
#include "Sensors/DayNight.h"

#include "Control/Pump.h"
#include "Control/Watering.h"
#include "Control/Refill.h"

#include "IO/LEDStrip.h"

static void sendSettingsPump(uint16_t id);
static void sendSettingsWatering(uint16_t id);
static void sendSettingsLights(uint16_t id);

static void sendEnvironment(uint16_t id);
static void sendControl(uint16_t id);
static void sendLights(uint16_t id);

enum StateResponse
{
    STATE_IDLE,

    // GET_STATE
    STATE_WAIT_ENV_ACK,
    STATE_WAIT_CONTROL_ACK,
    STATE_WAIT_LIGHTS_ACK,

    // GET_SETTINGS
    STATE_WAIT_SETTINGS_PUMP_ACK,
    STATE_WAIT_SETTINGS_WATERING_ACK,
    STATE_WAIT_SETTINGS_LIGHTS_ACK
};

static StateResponse stateResponse = STATE_IDLE;
static uint16_t stateId = 0;
static unsigned long responseTime = 0;

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
    // --------------------------------------------------
    // Transaction timeout
    // --------------------------------------------------

    if (stateResponse != STATE_IDLE &&
        millis() - responseTime >= COMMS_TIMEOUT_MS)
    {
        Serial.print("NANO: TRANSACTION TIMEOUT, state=");
        Serial.println((int)stateResponse);

        stateResponse = STATE_IDLE;
        stateId = 0;
        responseTime = 0;

        return;
    }

    if (!commsHasMessage())
        return;

    const char* command = commsGetCommand();
    const uint16_t id = commsGetId();

    // ==================================================
    // GET STATE
    // ==================================================

    if (strcmp(command, "get_state") == 0)
    {
        // --------------------------------------------------
        // New ID aborts current transaction.
        // --------------------------------------------------

        if (stateResponse != STATE_IDLE && id != stateId)
        {
            stateResponse = STATE_IDLE;
            stateId = 0;
            responseTime = 0;
        }

        // --------------------------------------------------
        // Start new transaction
        // --------------------------------------------------

        if (stateResponse == STATE_IDLE)
        {
            stateId = id;
            stateResponse = STATE_WAIT_ENV_ACK;
            responseTime = millis();

            sendEnvironment(id);
        }

        // --------------------------------------------------
        // Same ID = retransmit current block
        // --------------------------------------------------

        else if (id == stateId)
        {
            responseTime = millis();

            if (stateResponse == STATE_WAIT_ENV_ACK)
            {
                sendEnvironment(id);
            }
            else if (stateResponse == STATE_WAIT_CONTROL_ACK)
            {
                sendControl(id);
            }
            else if (stateResponse == STATE_WAIT_LIGHTS_ACK)
            {
                sendLights(id);
            }
        }
    }

    // ==================================================
    // GET SETTINGS
    // ==================================================

    else if (strcmp(command, "get_settings") == 0)
    {
        // --------------------------------------------------
        // New ID aborts current transaction.
        // --------------------------------------------------

        if (stateResponse != STATE_IDLE && id != stateId)
        {
            stateResponse = STATE_IDLE;
            stateId = 0;
            responseTime = 0;
        }

        // --------------------------------------------------
        // Start new transaction
        // --------------------------------------------------

        if (stateResponse == STATE_IDLE)
        {
            stateId = id;
            stateResponse = STATE_WAIT_SETTINGS_PUMP_ACK;
            responseTime = millis();

            sendSettingsPump(id);
        }

        // --------------------------------------------------
        // Same ID = retransmit current block
        // --------------------------------------------------

        else if (id == stateId)
        {
            responseTime = millis();

            if (stateResponse == STATE_WAIT_SETTINGS_PUMP_ACK)
            {
                sendSettingsPump(id);
            }
            else if (stateResponse == STATE_WAIT_SETTINGS_WATERING_ACK)
            {
                sendSettingsWatering(id);
            }
            else if (stateResponse == STATE_WAIT_SETTINGS_LIGHTS_ACK)
            {
                sendSettingsLights(id);
            }
        }
    }

    // ==================================================
    // ACK
    // ==================================================

    else if (strcmp(command, "ack") == 0)
    {
        if (id == stateId)
        {
            // --------------------------------------------------
            // GET_STATE
            // --------------------------------------------------

            if (stateResponse == STATE_WAIT_ENV_ACK)
            {
                stateResponse = STATE_WAIT_CONTROL_ACK;
                responseTime = millis();

                sendControl(id);
            }
            else if (stateResponse == STATE_WAIT_CONTROL_ACK)
            {
                stateResponse = STATE_WAIT_LIGHTS_ACK;
                responseTime = millis();

                sendLights(id);
            }
            else if (stateResponse == STATE_WAIT_LIGHTS_ACK)
            {
                stateResponse = STATE_IDLE;
                stateId = 0;
                responseTime = 0;
            }

            // --------------------------------------------------
            // GET_SETTINGS
            // --------------------------------------------------

            else if (stateResponse == STATE_WAIT_SETTINGS_PUMP_ACK)
            {
                stateResponse = STATE_WAIT_SETTINGS_WATERING_ACK;
                responseTime = millis();

                sendSettingsWatering(id);
            }
            else if (stateResponse == STATE_WAIT_SETTINGS_WATERING_ACK)
            {
                stateResponse = STATE_WAIT_SETTINGS_LIGHTS_ACK;
                responseTime = millis();

                sendSettingsLights(id);
            }
            else if (stateResponse == STATE_WAIT_SETTINGS_LIGHTS_ACK)
            {
                stateResponse = STATE_IDLE;
                stateId = 0;
                responseTime = 0;
            }
        }
    }

    // ==================================================
    // UNKNOWN COMMAND
    // ==================================================

    else
    {
        // Ignore silently.
    }

    commsClearMessage();
}

// --------------------------------------------------
// GET_STATE: Environment
// --------------------------------------------------

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
}

// --------------------------------------------------
// GET_STATE: Control
// --------------------------------------------------

void sendControl(uint16_t id)
{
    JsonDocument document;

    document["id"] = id;
    document["cmd"] = "control";

    document["pump"] = pumpGetState() >= 2;
    document["watering"] = wateringGetState() >= 2;
    document["refill"] = refillGetState() >= 2;
    document["empty"] = refillIsEmpty();
    document["dayNight"] = getDayState() == DAY;

    commsSendJson(document);
}

// --------------------------------------------------
// GET_STATE: Lights
// --------------------------------------------------

void sendLights(uint16_t id)
{
    JsonDocument document;

    document["id"] = id;
    document["cmd"] = "lights";

    document["loop"] = ledStripIsLoop();

    if (!ledStripIsLoop())
    {
        uint8_t hue;
        uint8_t saturation;
        uint8_t value;

        getLedStripColor(hue, saturation, value);

        document["H"] = hue;
        document["S"] = saturation;
        document["V"] = value;
    }

    commsSendJson(document);
}

// --------------------------------------------------
// GET_SETTINGS: Pump
// --------------------------------------------------

void sendSettingsPump(uint16_t id)
{
    JsonDocument document;

    document["id"] = id;
    document["cmd"] = "pump";

    document["dayPeriod"] = settings.pump.dayPeriod;
    document["dayRuntime"] = settings.pump.dayRunTime;
    document["nightPeriod"] = settings.pump.nightPeriod;
    document["nightRuntime"] = settings.pump.nightRunTime;

    commsSendJson(document);
}

// --------------------------------------------------
// GET_SETTINGS: Watering
// --------------------------------------------------

void sendSettingsWatering(uint16_t id)
{
    JsonDocument document;

    document["id"] = id;
    document["cmd"] = "watering";

    document["hour"] = settings.water.hour;
    document["minute"] = settings.water.minute;
    document["periodDays"] = settings.water.periodDays;
    document["runtime"] = settings.water.runTime;

    commsSendJson(document);
}

// --------------------------------------------------
// GET_SETTINGS: Lights
// --------------------------------------------------

void sendSettingsLights(uint16_t id)
{
    JsonDocument document;

    document["id"] = id;
    document["cmd"] = "lights";

    document["enabled"] = settings.led.enabled;
    document["loop"] = settings.led.loopMode;
    document["rate"] = settings.led.rate;

    uint8_t hue;
    uint8_t saturation;
    uint8_t value;

    getLedStripColor(hue, saturation, value);

    document["H"] = hue;
    document["S"] = saturation;
    document["V"] = value;

    commsSendJson(document);
}