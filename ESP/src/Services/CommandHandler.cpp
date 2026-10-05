#include "Services/CommandHandler.h"

#include <ArduinoJson.h>
#include <string.h>

#include "Config.h"

#include "Services/Comms.h"
#include "Services/CommandValidation.h"
#include "Services/CommandLoader.h"

#include "Web/WebState.h"
#include "Web/WebSettings.h"
#include "Web/WebControl.h"

// --------------------------------------------------
// Response state
// --------------------------------------------------

enum StateResponse
{
    STATE_IDLE,

    // GET_STATE
    STATE_WAIT_ENV,
    STATE_WAIT_CONTROL,
    STATE_WAIT_LEDS,

    // GET_SETTINGS
    STATE_WAIT_SETTINGS_PUMP,
    STATE_WAIT_SETTINGS_WATERING,
    STATE_WAIT_SETTINGS_LEDS,

    // SET_SETTINGS
    STATE_WAIT_SET_SETTINGS_PUMP_ACK,
    STATE_WAIT_SET_SETTINGS_WATERING_ACK,
    STATE_WAIT_SET_SETTINGS_LEDS_ACK,

    // SET_PUMP
    STATE_WAIT_SET_PUMP_ACK,

    // SET_WATERING
    STATE_WAIT_SET_WATERING_ACK,

    // SET_LEDS
    STATE_WAIT_SET_LEDS_ACK
};

static StateResponse stateResponse;
static Request activeRequest;

static uint16_t nextId; // Next request ID to be generated. 0 is reserved and is not a valid request ID.
static uint16_t stateId; // Current request ID for the active transaction. Readonly, used to match responses from the Nano.

static unsigned long responseTime; // Time when the last request was sent. Used to detect transaction timeouts.

static unsigned long testPollingTime;
static byte testPollingStep;

// --------------------------------------------------
// Internal functions
// --------------------------------------------------

static uint16_t generateId();

static void sendActiveRequest();
static void restartActiveRequest();

static void processNanoMessage();

// --------------------------------------------------
// Begin
// --------------------------------------------------

void commandHandlerBegin()
{
    stateResponse = STATE_IDLE;
    activeRequest.command = REQUEST_NONE;
    activeRequest.priority = 255;
    
    nextId = 1;
    stateId = 0;

    responseTime = 0;
    testPollingTime = millis();
    testPollingStep = 1;
}

// --------------------------------------------------
// Update
// --------------------------------------------------

void commandHandlerUpdate()
{
    if (commsHasMessage())
    {
        processNanoMessage();
        commsClearMessage();
    }

    const unsigned long now = millis();

    // --------------------------------------------------
    // Transaction timeout
    //
    // Initial states:
    //   - resend the complete request.
    //
    // Intermediate states:
    //   - GET_STATE: abort transaction.
    //   - GET_SETTINGS: restart complete transaction.
    //
    // We never try to resend an ACK.
    // The Nano retransmission mechanism already handles
    // the previous response. A new request gets a new ID.
    // --------------------------------------------------

    if (stateResponse != STATE_IDLE && now - responseTime >= COMMS_TIMEOUT_MS)
    {
        switch (stateResponse)
        {
            // ------------------------------------------
            // GET_STATE
            // ------------------------------------------

            case STATE_WAIT_ENV:
                restartActiveRequest();
                return;

            case STATE_WAIT_CONTROL:
            case STATE_WAIT_LEDS:

                stateResponse = STATE_IDLE;
                stateId = 0;
                responseTime = 0;
                activeRequest.command = REQUEST_NONE;
                return;

            // ------------------------------------------
            // GET_SETTINGS
            // ------------------------------------------

            case STATE_WAIT_SETTINGS_PUMP:
            case STATE_WAIT_SETTINGS_WATERING:
            case STATE_WAIT_SETTINGS_LEDS:

                restartActiveRequest();
                return;

            // ------------------------------------------
            // SET_SETTINGS
            // ------------------------------------------

            case STATE_WAIT_SET_SETTINGS_PUMP_ACK:
            case STATE_WAIT_SET_SETTINGS_WATERING_ACK:
            case STATE_WAIT_SET_SETTINGS_LEDS_ACK:
            
                sendActiveRequest();
                return;

            // ------------------------------------------
            // SET_PUMP / SET_WATERING / SET_LEDS
            // ------------------------------------------

            case STATE_WAIT_SET_PUMP_ACK:
            case STATE_WAIT_SET_WATERING_ACK:
            case STATE_WAIT_SET_LEDS_ACK:

                restartActiveRequest();
                return;

            default:

                stateResponse = STATE_IDLE;
                stateId = 0;
                responseTime = 0;
                activeRequest.command = REQUEST_NONE;

                return;
        }
    }

    // --------------------------------------------------
    // An active transaction is still running.
    // Do not start another periodic request.
    // --------------------------------------------------

    if (stateResponse != STATE_IDLE)
        return;

    // --------------------------------------------------
    // Periodic polling.
    //
    // Test only, will be triggered by web module.
    // --------------------------------------------------

    // --------------------------------------------------
    // Test polling
    // --------------------------------------------------

    if (stateResponse == STATE_IDLE)
    {
        if (now - testPollingTime >= TEST_POLLING_PERIOD_MS)
        {
            testPollingTime = now;

            switch (testPollingStep)
            {
                case 1:
                    request(REQUEST_GET_STATE, 10);
                    testPollingStep = 2;
                    break;

                case 2:
                    request(REQUEST_GET_SETTINGS, 5);
                    testPollingStep = 3;
                    break;

                case 3:
                    request(REQUEST_SET_SETTINGS, 4);
                    testPollingStep = 4;
                    break;

                case 4:
                    request(REQUEST_SET_PUMP, 3);
                    testPollingStep = 5;
                    break;

                case 5:
                    request(REQUEST_SET_WATERING, 2);
                    testPollingStep = 6;
                    break;

                case 6:
                    request(REQUEST_SET_LEDS, 30);
                    testPollingStep = 1;
                    break;
            }
        }
    }
}

// --------------------------------------------------
// Request
// --------------------------------------------------

bool request(RequestCommand command, RequestPriority priority)
{
    // Priority 0 is reserved and is not a valid request priority.
    if (priority == 0)
        return false;

    // --------------------------------------------------
    // No active request
    // --------------------------------------------------

    if (activeRequest.command == REQUEST_NONE)
    {
        activeRequest = { command, priority };

        sendActiveRequest();

        return true;
    }

    // --------------------------------------------------
    // Active request
    //
    // A lower numerical value means higher priority.
    //
    // new priority < active priority
    //     -> abort current transaction
    //
    // new priority >= active priority
    //     -> ignore new request
    // --------------------------------------------------

    if (priority >= activeRequest.priority)
        return false;

    // The Nano is not notified that the transaction was
    // aborted. A new ID makes all responses belonging to
    // the previous transaction obsolete.

    stateResponse = STATE_IDLE;
    stateId = 0;
    responseTime = 0;

    activeRequest = { command, priority };

    sendActiveRequest();

    return true;
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
// Process Nano message
// --------------------------------------------------

static void processNanoMessage()
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
                webState.environment.temperature = temperature;
                webState.environment.humidity = humidity;

                loadCommandAck(id);

                responseTime = millis();
                stateResponse = STATE_WAIT_CONTROL;
            }
        }

        return;
    }

    if (stateResponse == STATE_WAIT_CONTROL)
    {
        if (strcmp(command, "environment") == 0)
        {
            // ACK lost.
            // Nano is retransmitting environment.

            loadCommandAck(id);

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
                webState.control.pump = pump;
                webState.control.watering = watering;
                webState.control.refill = refill;
                webState.control.empty = empty;
                webState.control.dayNight = dayNight;

                loadCommandAck(id);

                responseTime = millis();
                stateResponse = STATE_WAIT_LEDS;
            }
        }

        return;
    }

    if (stateResponse == STATE_WAIT_LEDS)
    {
        if (strcmp(command, "control") == 0)
        {
            // ACK lost.
            loadCommandAck(id);

            return;
        }

        if (strcmp(command, "leds") == 0)
        {
            bool loop;
            uint8_t hue;

            if (validateLeds(
                    message,
                    loop,
                    hue))
            {
                webSettings.leds.loop = loop;
                webSettings.leds.hue = hue;

                loadCommandAck(id);

                stateResponse = STATE_IDLE;
                stateId = 0;
                activeRequest.command = REQUEST_NONE;
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
                webSettings.pump.dayPeriod = dayPeriod;
                webSettings.pump.dayRuntime = dayRuntime;
                webSettings.pump.nightPeriod = nightPeriod;
                webSettings.pump.nightRuntime = nightRuntime;

                loadCommandAck(id);

                responseTime = millis();
                stateResponse = STATE_WAIT_SETTINGS_WATERING;
            }
        }

        return;
    }

    if (stateResponse == STATE_WAIT_SETTINGS_WATERING)
    {
        if (strcmp(command, "pump") == 0)
        {
            // ACK lost.
            loadCommandAck(id);

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
                webSettings.watering.hour = hour;
                webSettings.watering.minute = minute;
                webSettings.watering.periodDays = periodDays;
                webSettings.watering.runtime = runTime;

                loadCommandAck(id);

                responseTime = millis();
                stateResponse = STATE_WAIT_SETTINGS_LEDS;
            }
        }

        return;
    }

    if (stateResponse == STATE_WAIT_SETTINGS_LEDS)
    {
        if (strcmp(command, "watering") == 0)
        {
            // ACK lost.
            loadCommandAck(id);

            return;
        }

        if (strcmp(command, "leds") == 0)
        {
            bool enabled;
            bool loop;
            uint8_t rate;
            uint8_t hue;
            uint8_t saturation;
            uint8_t value;

            if (validateLedsSettings(
                    message,
                    enabled,
                    loop,
                    rate,
                    hue,
                    saturation,
                    value))
            {
                webSettings.leds.enabled = enabled;
                webSettings.leds.loop = loop;
                webSettings.leds.rate = rate;
                webSettings.leds.hue = hue;
                webSettings.leds.saturation = saturation;
                webSettings.leds.value = value;

                loadCommandAck(id);

                stateResponse = STATE_IDLE;
                stateId = 0;
                activeRequest.command = REQUEST_NONE;
            }
        }

        return;
    }

    // ==================================================
    // ACK
    // ==================================================

    if (strcmp(command, "ack") == 0)
    {
        if (id == stateId)
        {
            // --------------------------------------------------
            // SET_SETTINGS
            // --------------------------------------------------

            if (stateResponse == STATE_WAIT_SET_SETTINGS_PUMP_ACK)
            {
                stateId = generateId();
                loadCommandSetSettingsWatering(stateId);

                responseTime = millis();
                stateResponse = STATE_WAIT_SET_SETTINGS_WATERING_ACK;
            }
            else if (stateResponse == STATE_WAIT_SET_SETTINGS_WATERING_ACK)
            {
                stateId = generateId();
                loadCommandSetSettingsLeds(stateId);

                responseTime = millis();
                stateResponse = STATE_WAIT_SET_SETTINGS_LEDS_ACK;
            }
            else if (stateResponse == STATE_WAIT_SET_SETTINGS_LEDS_ACK)
            {
                stateResponse = STATE_IDLE;
                stateId = 0;
                activeRequest.command = REQUEST_NONE;
            }

            // --------------------------------------------------
            // SET_PUMP
            // --------------------------------------------------

            else if (stateResponse == STATE_WAIT_SET_PUMP_ACK)
            {
                stateResponse = STATE_IDLE;
                stateId = 0;
                activeRequest.command = REQUEST_NONE;
            }

            // --------------------------------------------------
            // SET_WATERING
            // --------------------------------------------------

            else if (stateResponse == STATE_WAIT_SET_WATERING_ACK)
            {
                stateResponse = STATE_IDLE;
                stateId = 0;
                activeRequest.command = REQUEST_NONE;
            }

            // --------------------------------------------------
            // SET_LEDS
            // --------------------------------------------------

            else if (stateResponse == STATE_WAIT_SET_LEDS_ACK)
            {
                stateResponse = STATE_IDLE;
                stateId = 0;
                activeRequest.command = REQUEST_NONE;
            }
        }

        return;
    }
}

// --------------------------------------------------
// Send active request
// --------------------------------------------------

static void sendActiveRequest()
{
    switch (activeRequest.command)
    {
        case REQUEST_GET_STATE:

            stateId = generateId();

            loadCommandGetState(stateId);

            stateResponse = STATE_WAIT_ENV;
            responseTime = millis();

            break;

        case REQUEST_GET_SETTINGS:

            stateId = generateId();

            loadCommandGetSettings(stateId);

            stateResponse = STATE_WAIT_SETTINGS_PUMP;
            responseTime = millis();

            break;

        case REQUEST_SET_SETTINGS:

            stateId = generateId();

            loadCommandSetSettingsPump(stateId);

            stateResponse = STATE_WAIT_SET_SETTINGS_PUMP_ACK;
            responseTime = millis();

            break;

        case REQUEST_SET_PUMP:

            stateId = generateId();

            loadCommandSetPump(stateId);

            stateResponse = STATE_WAIT_SET_PUMP_ACK;
            responseTime = millis();

            break;

        case REQUEST_SET_WATERING:

            stateId = generateId();

            loadCommandSetWatering(stateId);

            stateResponse = STATE_WAIT_SET_WATERING_ACK;
            responseTime = millis();

            break;

        case REQUEST_SET_LEDS:

            stateId = generateId();

            loadCommandSetLeds(stateId);

            stateResponse = STATE_WAIT_SET_LEDS_ACK;
            responseTime = millis();

            break;

        case REQUEST_NONE:
            break;
    }
}

// --------------------------------------------------
// Restart active request
// --------------------------------------------------

static void restartActiveRequest()
{
    RequestCommand command = activeRequest.command;
    RequestPriority priority = activeRequest.priority;

    stateResponse = STATE_IDLE;
    stateId = 0;
    responseTime = 0;
    activeRequest.command = REQUEST_NONE;

    request(command, priority);
}
