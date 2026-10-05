#ifndef COMMAND_VALIDATION_H
#define COMMAND_VALIDATION_H

#include <ArduinoJson.h>
#include <stdint.h>

bool validatePumpSettings(
    JsonDocument& message,
    uint16_t& dayPeriod,
    uint8_t& dayRuntime,
    uint16_t& nightPeriod,
    uint8_t& nightRuntime);

bool validateWateringSettings(
    JsonDocument& message,
    uint8_t& hour,
    uint8_t& minute,
    uint8_t& periodDays,
    uint8_t& runtime);

bool validateLedsSettings(
    JsonDocument& message,
    bool& enabled,
    bool& loop,
    uint8_t& rate,
    uint8_t& hue,
    uint8_t& saturation,
    uint8_t& value);

bool validateSetPump(
    JsonDocument& message,
    bool& manualOperation);

bool validateSetWatering(
    JsonDocument& message,
    bool& manualOperation);

bool validateSetLeds(
    JsonDocument& message,
    bool& loop,
    uint8_t& hue);

#endif