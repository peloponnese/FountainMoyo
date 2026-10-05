#include "Services/CommandHandler.h"

#include <string.h>
#include <ArduinoJson.h>

#include "Config.h"

#include "Types.h"

#include "Control/Pump.h"
#include "Control/Watering.h"

#include "Services/Comms.h"
#include "Services/CommandValidation.h"
#include "Services/CommandLoader.h"
#include "Services/Settings.h"

enum StateResponse
{
    STATE_IDLE,

    // GET_STATE
    STATE_WAIT_ENV_ACK,
    STATE_WAIT_CONTROL_ACK,
    STATE_WAIT_LEDS_ACK,

    // GET_SETTINGS
    STATE_WAIT_SETTINGS_PUMP_ACK,
    STATE_WAIT_SETTINGS_WATERING_ACK,
    STATE_WAIT_SETTINGS_LEDS_ACK
};

static StateResponse stateResponse; // Current state of the response transaction
static uint16_t stateId; // Current transaction ID (used to match ACKs to the correct transaction)
static unsigned long responseTime; // Time when the last response was received

// --------------------------------------------------
// Internal functions
// --------------------------------------------------

static void processEspMessage();

// --------------------------------------------------
// Begin
// --------------------------------------------------

void commandHandlerBegin()
{
    stateResponse = STATE_IDLE;
    stateId = 0;
    responseTime = 0;
}

// --------------------------------------------------
// Update
// --------------------------------------------------

void commandHandlerUpdate()
{
    if (commsHasMessage())
    {
        processEspMessage();
        commsClearMessage();
    }

    const unsigned long now = millis();

    // --------------------------------------------------
    // Transaction timeout
    // --------------------------------------------------

    if (stateResponse != STATE_IDLE && now - responseTime >= COMMS_TIMEOUT_MS)
    {
        Serial.print("NANO: TRANSACTION TIMEOUT, state=");
        Serial.println((int)stateResponse);

        stateResponse = STATE_IDLE;
        stateId = 0;
        responseTime = 0;

        return;
    }
}

// --------------------------------------------------
// Process ESP message
// --------------------------------------------------

static void processEspMessage()
{
    JsonDocument& message = commsGetMessage();

    if (!message["id"].is<uint16_t>())
        return;

    if (!message["cmd"].is<const char*>())
        return;

    const uint16_t id = message["id"].as<uint16_t>();
    const char* command = message["cmd"];

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
            stateId = 0;
            responseTime = 0;
            stateResponse = STATE_IDLE;
        }

        // --------------------------------------------------
        // Start new transaction
        // --------------------------------------------------

        if (stateResponse == STATE_IDLE)
        {
            stateId = id;
            loadCommandEnvironment(id);

            responseTime = millis();
            stateResponse = STATE_WAIT_ENV_ACK;
        }

        // --------------------------------------------------
        // Same ID = retransmit current block
        // --------------------------------------------------

        else if (id == stateId)
        {
            if (stateResponse == STATE_WAIT_ENV_ACK)
            {
                loadCommandEnvironment(id);
            }
            else if (stateResponse == STATE_WAIT_CONTROL_ACK)
            {
                loadCommandControl(id);
            }
            else if (stateResponse == STATE_WAIT_LEDS_ACK)
            {
                loadCommandLeds(id);
            }

            responseTime = millis();
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

            loadCommandSettingsPump(id);
        }

        // --------------------------------------------------
        // Same ID = retransmit current block
        // --------------------------------------------------

        else if (id == stateId)
        {
            responseTime = millis();

            if (stateResponse == STATE_WAIT_SETTINGS_PUMP_ACK)
            {
                loadCommandSettingsPump(id);
            }
            else if (stateResponse == STATE_WAIT_SETTINGS_WATERING_ACK)
            {
                loadCommandSettingsWatering(id);
            }
            else if (stateResponse == STATE_WAIT_SETTINGS_LEDS_ACK)
            {
                loadCommandSettingsLeds(id);
            }
        }
    }

    // ==================================================
    // SET SETTINGS PUMP
    // ==================================================

    else if (strcmp(command, "set_settings_pump") == 0)
    {
        if (stateResponse == STATE_IDLE)
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
                settings.pump.dayPeriod = dayPeriod;
                settings.pump.dayRunTime = dayRuntime;
                settings.pump.nightPeriod = nightPeriod;
                settings.pump.nightRunTime = nightRuntime;
                settingsValidate();

                loadCommandAck(id);
            }
        }
    }

    // ==================================================
    // SET SETTINGS WATERING
    // ==================================================

    else if (strcmp(command, "set_settings_watering") == 0)
    {
        if (stateResponse == STATE_IDLE)
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
                settings.water.hour = hour;
                settings.water.minute = minute;
                settings.water.periodDays = periodDays;
                settings.water.runTime = runTime;
                settingsValidate();

                loadCommandAck(id);
            }
        }
    }

    // ==================================================
    // SET SETTINGS LEDS
    // ==================================================

    else if (strcmp(command, "set_settings_leds") == 0)
    {
        if (stateResponse == STATE_IDLE)
        {
            bool enabled;
            bool loopMode;
            uint8_t rate;
            uint8_t hue;
            uint8_t saturation;
            uint8_t value;
            
            if (validateLedsSettings(
                message,
                enabled,
                loopMode,
                rate,
                hue,
                saturation,
                value
            ))
            {
                settings.leds.enabled = enabled;
                settings.leds.loopMode = loopMode;
                settings.leds.rate = rate;
                settings.leds.hue = hue;
                settings.leds.saturation = saturation;
                settings.leds.value = value;
                settingsValidate();

                loadCommandAck(id);
            }
        }
    }

    // ==================================================
    // SET PUMP
    // ==================================================

    else if (strcmp(command, "set_pump") == 0)
    {
        if (stateResponse == STATE_IDLE)
        {
            bool manualOperation;

            if (validateSetPump(
                message,
                manualOperation
            ))
            {   
                State pumpState = pumpGetState();
                // set MANUAL for dayRuntime/nightRuntime
                if (manualOperation && pumpState == OFF)
                {
                    pumpSetRunningExternal();
                }
                else if (!manualOperation && (pumpState == MANUAL || pumpState == RUNNING))
                {
                    pumpSetOffExternal();
                }

                loadCommandAck(id);
            }
        }
    }

    // ==================================================
    // SET WATERING
    // ==================================================

    else if (strcmp(command, "set_watering") == 0)
    {
        if (stateResponse == STATE_IDLE)
        {
            bool manualOperation;

            if (validateSetWatering(
                message,
                manualOperation
            ))
            {   
                State wateringState = wateringGetState();
                // set MANUAL for runTime
                if (manualOperation && wateringState == OFF)
                {
                    wateringSetRunningExternal();
                }
                else if (!manualOperation && (wateringState == MANUAL || wateringState == RUNNING))
                {
                    wateringSetOffExternal();
                }

                loadCommandAck(id);
            }
        }
    }

    // ==================================================
    // SET LEDS
    // ==================================================

    else if (strcmp(command, "set_leds") == 0)
    {
        if (stateResponse == STATE_IDLE)
        {
            bool loopMode;
            uint8_t hue;

            if (validateSetLeds(
                message,
                loopMode,
                hue
            ))
            {
                settings.leds.loopMode = loopMode;
                settings.leds.hue = hue;
                settingsValidate();

                loadCommandAck(id);
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
                loadCommandControl(id);

                responseTime = millis();
                stateResponse = STATE_WAIT_CONTROL_ACK;
            }
            else if (stateResponse == STATE_WAIT_CONTROL_ACK)
            {
                loadCommandLeds(id);

                responseTime = millis();
                stateResponse = STATE_WAIT_LEDS_ACK;
            }
            else if (stateResponse == STATE_WAIT_LEDS_ACK)
            {
                stateId = 0;
                responseTime = 0;
                stateResponse = STATE_IDLE;
            }

            // --------------------------------------------------
            // GET_SETTINGS
            // --------------------------------------------------

            else if (stateResponse == STATE_WAIT_SETTINGS_PUMP_ACK)
            {
                loadCommandSettingsWatering(id);

                responseTime = millis();
                stateResponse = STATE_WAIT_SETTINGS_WATERING_ACK;
            }
            else if (stateResponse == STATE_WAIT_SETTINGS_WATERING_ACK)
            {
                loadCommandSettingsLeds(id);

                responseTime = millis();
                stateResponse = STATE_WAIT_SETTINGS_LEDS_ACK;
            }
            else if (stateResponse == STATE_WAIT_SETTINGS_LEDS_ACK)
            {
                stateId = 0;
                responseTime = 0;
                stateResponse = STATE_IDLE;
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
}

// --------------------------------------------------
// Reply
// --------------------------------------------------

void commandReply()
{
    if (!commandHasReply())
        return;

    JsonDocument& reply = commandGetReply();

    commsSend(reply);

    commandClearReply();
}