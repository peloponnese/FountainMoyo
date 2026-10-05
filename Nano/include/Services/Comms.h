#ifndef COMMS_H
#define COMMS_H

#include <Arduino.h>
#include <ArduinoJson.h>

void commsBegin();

void commsUpdate();

bool commsHasMessage();

JsonDocument& commsGetMessage();

void commsClearMessage();

void commsSend(JsonDocument& document);

#endif