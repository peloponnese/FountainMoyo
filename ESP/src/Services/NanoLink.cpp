#include "Services/NanoLink.h"

#include <ArduinoJson.h>
#include <string.h>

#include "Config.h"
#include "Services/Comms.h"
#include "Services/NanoLinkValidation.h"

enum StateResponse
{
    STATE_IDLE,

    // GET_STATE
    STATE_WAIT_ENV,
    STATE_WAIT_CONTROL,
    STATE_WAIT_LIGHTS,

    // GET_SETTINGS
    STATE_WAIT_SETTINGS_PUMP,
    STATE_WAIT_SETTINGS_WATERING,
    STATE_WAIT_SETTINGS_LIGHTS
};

static StateResponse stateResponse = STATE_IDLE;

static uint16_t nextId = 1;
static uint16_t stateId = 0;

static unsigned long requestTime = 0;
static unsigned long lastStateRequest = 0;
static unsigned long lastSettingsRequest = 0;

static void sendStateRequest();
static void sendSettingsRequest();

static void processNanoResponse();

// --------------------------------------------------
// Begin
// --------------------------------------------------

void nanoLinkBegin()
{
    nextId = 1;

    stateResponse = STATE_IDLE;
    stateId = 0;

    requestTime = 0;

    lastStateRequest = millis();
    lastSettingsRequest = millis();
}

// --------------------------------------------------
// Update
// --------------------------------------------------

void nanoLinkUpdate()
{
    if (commsHasMessage())
    {
        processNanoResponse();
        commsClearMessage();
    }

    // --------------------------------------------------
    // Waiting for first response to current request
    // --------------------------------------------------

    if (stateResponse == STATE_WAIT_ENV ||
        stateResponse == STATE_WAIT_SETTINGS_PUMP)
    {
        if (millis() - requestTime >= COMMS_TIMEOUT_MS)
        {
            if (stateResponse == STATE_WAIT_ENV)
                sendStateRequest();
            else
                sendSettingsRequest();
        }

        return;
    }

    // --------------------------------------------------
    // Remaining stages:
    // Nano owns retransmission.
    // --------------------------------------------------

    if (stateResponse != STATE_IDLE)
        return;

    // --------------------------------------------------
    // Periodic GET_STATE
    // --------------------------------------------------

    if (millis() - lastStateRequest >= STATE_REQUEST_PERIOD_MS)
    {
        lastStateRequest = millis();
        sendStateRequest();
        return;
    }

    // --------------------------------------------------
    // Periodic GET_SETTINGS
    // Test mechanism only.
    // Will later be triggered by Web Module.
    // --------------------------------------------------

    if (millis() - lastSettingsRequest >= SETTINGS_REQUEST_PERIOD_MS)
    {
        lastSettingsRequest = millis();
        sendSettingsRequest();
        return;
    }
}

// --------------------------------------------------
// ID generator
// --------------------------------------------------

static uint16_t generateId()
{
    uint16_t id = nextId++;

    if (nextId == 0)
        nextId = 1;

    return id;
}

// --------------------------------------------------
// Send GET_STATE
// --------------------------------------------------

static void sendStateRequest()
{
    JsonDocument document;

    stateId = generateId();

    document["id"] = stateId;
    document["cmd"] = "get_state";
    document["type"] = "periodic";

    commsSend(document);

    stateResponse = STATE_WAIT_ENV;
    requestTime = millis();
}

// --------------------------------------------------
// Send GET_SETTINGS
// --------------------------------------------------

static void sendSettingsRequest()
{
    JsonDocument document;

    stateId = generateId();

    document["id"] = stateId;
    document["cmd"] = "get_settings";

    commsSend(document);

    stateResponse = STATE_WAIT_SETTINGS_PUMP;
    requestTime = millis();
}

// --------------------------------------------------
// Process Nano response
// --------------------------------------------------

static void processNanoResponse()
{
    JsonDocument& message = commsGetMessage();

    if (!message["id"].is<uint16_t>())
        return;

    if (!message["cmd"].is<const char*>())
        return;

    const uint16_t id = message["id"].as<uint16_t>();
    const char* command = message["cmd"];

    if (id != stateId)
        return;

    // ==================================================
    // GET_STATE
    // ==================================================

    if (stateResponse == STATE_WAIT_ENV)
    {
        if (strcmp(command, "environment") == 0)
        {
            float temperature;
            float humidity;
            bool valid;

            if (validateEnvironment(
                    message,
                    temperature,
                    humidity,
                    valid))
            {
                // TODO:
                // webState.temperature = temperature;
                // webState.humidity = humidity;
                // webState.environmentValid = valid;

                commsSendAck(id);

                stateResponse = STATE_WAIT_CONTROL;
            }
        }

        return;
    }

    if (stateResponse == STATE_WAIT_CONTROL)
    {
        if (strcmp(command, "environment") == 0)
        {
            // ACK perdido.
            // Nano está retransmitiendo environment.
            commsSendAck(id);
            return;
        }

        if (strcmp(command, "control") == 0)
        {
            bool pump;
            bool watering;
            bool refill;
            bool empty;
            bool dayNight;

            if (validateControl(
                    message,
                    pump,
                    watering,
                    refill,
                    empty,
                    dayNight))
            {
                // TODO:
                // webState.pump = pump;
                // webState.watering = watering;
                // webState.refill = refill;
                // webState.empty = empty;
                // webState.dayNight = dayNight;

                commsSendAck(id);

                stateResponse = STATE_WAIT_LIGHTS;
            }
        }

        return;
    }

    if (stateResponse == STATE_WAIT_LIGHTS)
    {
        if (strcmp(command, "control") == 0)
        {
            // ACK perdido.
            commsSendAck(id);
            return;
        }

        if (strcmp(command, "lights") == 0)
        {
            bool loop;
            uint8_t hue;
            uint8_t saturation;
            uint8_t value;

            if (validateLights(
                    message,
                    loop,
                    hue,
                    saturation,
                    value))
            {
                // TODO:
                // webState.lightsLoop = loop;
                // if (!loop)
                // {
                //     webState.hue = hue;
                //     webState.saturation = saturation;
                //     webState.value = value;
                // }

                commsSendAck(id);

                stateResponse = STATE_IDLE;
                stateId = 0;
            }
        }

        return;
    }

    // ==================================================
    // GET_SETTINGS
    // ==================================================

    if (stateResponse == STATE_WAIT_SETTINGS_PUMP)
    {
        if (strcmp(command, "pump") == 0)
        {
            uint16_t dayPeriod;
            uint8_t dayRuntime;
            uint16_t nightPeriod;
            uint8_t nightRuntime;

            if (validatePumpSettings(
                    message,
                    dayPeriod,
                    dayRuntime,
                    nightPeriod,
                    nightRuntime))
            {
                // TODO: actualizar webState/settings cache

                commsSendAck(id);

                stateResponse = STATE_WAIT_SETTINGS_WATERING;
            }
        }

        return;
    }

    if (stateResponse == STATE_WAIT_SETTINGS_WATERING)
    {
        if (strcmp(command, "pump") == 0)
        {
            // ACK perdido.
            commsSendAck(id);
            return;
        }

        if (strcmp(command, "watering") == 0)
        {
            uint8_t hour;
            uint8_t minute;
            uint8_t periodDays;
            uint8_t runTime;

            if (validateWateringSettings(
                message,
                hour,
                minute,
                periodDays,
                runTime))
            {
                // TODO: actualizar webState/settings cache

                commsSendAck(id);

                stateResponse = STATE_WAIT_SETTINGS_LIGHTS;
            }
        }

        return;
    }

    if (stateResponse == STATE_WAIT_SETTINGS_LIGHTS)
    {
        if (strcmp(command, "watering") == 0)
        {
            // ACK perdido.
            commsSendAck(id);
            return;
        }

        if (strcmp(command, "lights") == 0)
        {
            bool enabled;
            bool loop;
            uint8_t rate;
            uint8_t hue;
            uint8_t saturation;
            uint8_t value;

            if (validateLightsSettings(
                    message,
                    enabled,
                    loop,
                    rate,
                    hue,
                    saturation,
                    value))
            {
                // TODO: actualizar webState/settings cache

                commsSendAck(id);

                stateResponse = STATE_IDLE;
                stateId = 0;
            }
        }

        return;
    }
}