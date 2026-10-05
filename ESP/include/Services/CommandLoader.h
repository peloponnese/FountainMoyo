#ifndef COMMAND_LOADER_H
#define COMMAND_LOADER_H

#include <ArduinoJson.h>
#include <stdint.h>

// --------------------------------------------------
// Command preparation
// --------------------------------------------------

void loadCommandGetState(uint16_t id);
void loadCommandGetSettings(uint16_t id);

void loadCommandSetSettingsPump(uint16_t id);
void loadCommandSetSettingsWatering(uint16_t id);
void loadCommandSetSettingsLeds(uint16_t id);

void loadCommandSetPump(uint16_t id);
void loadCommandSetWatering(uint16_t id);
void loadCommandSetLeds(uint16_t id);

void loadCommandAck(uint16_t id);

// --------------------------------------------------
// Pending command
// --------------------------------------------------

bool commandHasReply();
JsonDocument& commandGetReply();
void commandClearReply();

#endif