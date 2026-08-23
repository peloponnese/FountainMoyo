#include "Services/CommandHandler.h"

#include <ArduinoJson.h>

#include "Services/Comms.h"
#include "Services/NanoLink.h"

static void handleCommand();

void commandHandlerBegin()
{
}

void commandHandlerUpdate()
{
    if (!commsHasMessage())
        return;

    handleCommand();

    commsClearMessage();
}

// --------------------------------------------------
// Command dispatcher
// --------------------------------------------------

static void handleCommand()
{
    JsonDocument& message = commsGetMessage();

    if (!message["cmd"].is<const char*>())
        return;

    const char* command = message["cmd"];

}