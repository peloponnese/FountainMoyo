#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Serial
constexpr uint32_t SERIAL_BAUD_RATE = 9600UL;

// Nano communication
constexpr uint16_t COMMS_BUFFER_SIZE = 192;
constexpr unsigned long COMMS_TIMEOUT_MS = 500UL;
constexpr byte NANO_MAX_RETRIES = 10;

// Periodic state request
constexpr unsigned long GET_STATE_PERIOD_MS = 5000UL;
constexpr unsigned long GET_SETTINGS_PERIOD_MS = 18750UL;
constexpr unsigned long SET_SETTINGS_PERIOD_MS = 36500UL;
constexpr unsigned long SET_PUMP_PERIOD_MS = 80500UL;
constexpr unsigned long SET_WATERING_PERIOD_MS = 146500UL;
constexpr unsigned long SET_LEDS_PERIOD_MS = 243500UL;

#endif