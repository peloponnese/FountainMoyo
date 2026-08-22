#ifndef COMMS_H
#define COMMS_H

#include <Arduino.h>
#include <ArduinoJson.h>

void commsBegin();

void commsUpdate();

bool commsHasMessage();

const char* commsGetCommand();

uint16_t commsGetId();

bool commsGetInt(const char* key, int& value);

bool commsGetByte(const char* key, byte& value);

bool commsGetBool(const char* key, bool& value);

void commsClearMessage();

void commsSendJson(JsonDocument& document);

void commsDebugJson(JsonDocument& document);

void commsSendOk(uint16_t id);

void commsSendCommError();

void commsSendUnknownCommand(uint16_t id);

#endif