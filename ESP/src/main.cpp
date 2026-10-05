#include <Arduino.h>

#include "Services/Comms.h"
#include "Services/CommandHandler.h"

void setup()
{
    commsBegin();
    commandHandlerBegin();

    delay(500);

    Serial.println();
    Serial.println("ESP COMMS READY");
}

void loop()
{
    commsUpdate();
    commandHandlerUpdate();
    commandReply();
}