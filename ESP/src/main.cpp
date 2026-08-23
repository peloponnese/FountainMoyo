#include <Arduino.h>

#include "Services/Comms.h"
#include "Services/CommandHandler.h"
#include "Services/NanoLink.h"

void setup()
{
    commsBegin();
    commandHandlerBegin();
    nanoLinkBegin();

    delay(500);

    Serial.println();
    Serial.println("ESP COMMS READY");
}

void loop()
{
    commsUpdate();
    nanoLinkUpdate();
    commandHandlerUpdate();
}