#include "Services/NanoLink.h"

#include <ArduinoJson.h>

#include "Config.h"
#include "Services/Comms.h"

static uint16_t nextId = 1;

static bool requestPending = false;
static uint16_t pendingId = 0;

static unsigned long requestTime = 0;
static unsigned long lastEnvironmentRequest = 0;

static void sendEnvironmentRequest();

static void retryRequest();

static void processNanoResponse();

void nanoLinkBegin()
{
    nextId = 1;

    requestPending = false;
    pendingId = 0;
    requestTime = 0;
    lastEnvironmentRequest = millis();
}

void nanoLinkUpdate()
{
    if (commsHasMessage())
    {
        processNanoResponse();
        commsClearMessage();
    }

    if (requestPending)
    {
        retryRequest();
    }

    if (!requestPending && millis() - lastEnvironmentRequest >= ENVIRONMENT_PERIOD_MS)
    {
        lastEnvironmentRequest = millis();

        nanoRequestEnvironment();
    }
}

// --------------------------------------------------
// Request environment
// --------------------------------------------------

void nanoRequestEnvironment()
{
    // Do not start another request while one is pending.
    if (requestPending)
        return;

    sendEnvironmentRequest();
}

// --------------------------------------------------
// Send environment request
// --------------------------------------------------

static void sendEnvironmentRequest()
{
    JsonDocument document;

    pendingId = nextId++;

    if (nextId == 0)
        nextId = 1;

    document["id"] = pendingId;
    document["cmd"] = "get_environment";
    document["type"] = "periodic";

    commsSend(document);

    requestPending = true;
    requestTime = millis();
}

// --------------------------------------------------
// Retry
// --------------------------------------------------

static void retryRequest()
{
    if (millis() - requestTime < REQUEST_TIMEOUT_MS)
        return;

    requestTime = millis();

    JsonDocument document;

    document["id"] = pendingId;
    document["cmd"] = "get_environment";
    document["type"] = "periodic";

    commsSend(document);
}

// --------------------------------------------------
// Process Nano response
// --------------------------------------------------

static void processNanoResponse()
{
    JsonDocument& message = commsGetMessage();

    if (!message["id"].is<uint16_t>())
        return;

    uint16_t id = message["id"];

    if (requestPending && id == pendingId)
    {
        requestPending = false;
    }
}