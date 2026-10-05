#ifndef COMMAND_LOADER_H
#define COMMAND_LOADER_H

#include <ArduinoJson.h>
#include <stdint.h>

// --------------------------------------------------
// Command preparation
// --------------------------------------------------

void loadCommandEnvironment(uint16_t id);
void loadCommandControl(uint16_t id);
void loadCommandLeds(uint16_t id);

void loadCommandSettingsPump(uint16_t id);
void loadCommandSettingsWatering(uint16_t id);
void loadCommandSettingsLeds(uint16_t id);

void loadCommandAck(uint16_t id);
void loadCommandCommError(uint16_t id);

// --------------------------------------------------
// Pending reply
// --------------------------------------------------

bool commandHasReply();
JsonDocument& commandGetReply();
void commandClearReply();

#endif