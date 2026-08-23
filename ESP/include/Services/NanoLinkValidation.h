#ifndef NANOLINKVALIDATION_H
#define NANOLINKVALIDATION_H

#include <ArduinoJson.h>
#include <stdint.h>

bool validateEnvironment(
    JsonDocument& message,
    float& temperature,
    float& humidity,
    bool& valid);

bool validateControl(
    JsonDocument& message,
    bool& pump,
    bool& watering,
    bool& refill,
    bool& empty,
    bool& dayNight);

bool validateLights(
    JsonDocument& message,
    bool& loop,
    uint8_t& hue,
    uint8_t& saturation,
    uint8_t& value);

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

bool validateLightsSettings(
    JsonDocument& message,
    bool& enabled,
    bool& loop,
    uint8_t& rate,
    uint8_t& hue,
    uint8_t& saturation,
    uint8_t& value);

#endif