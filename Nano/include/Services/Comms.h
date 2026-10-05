#ifndef COMMS_H
#define COMMS_H

#include <ArduinoJson.h>

void commsBegin();
void commsUpdate();

bool commsHasMessage();
void commsClearMessage();

JsonDocument& commsGetMessage();
void commsSend(JsonDocument& document);

bool commsCanRunBlockingOperation();

#endif