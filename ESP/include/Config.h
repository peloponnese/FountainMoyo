#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Serial
constexpr uint32_t SERIAL_BAUD_RATE = 9600UL;

// Nano communication
constexpr uint16_t COMMS_BUFFER_SIZE = 192;
constexpr unsigned long COMMS_TIMEOUT_MS = 2000UL;
constexpr byte NANO_MAX_RETRIES = 10;

// Periodic state request
constexpr unsigned long GET_STATE_PERIOD_MS = 4000UL;
constexpr unsigned long GET_SETTINGS_PERIOD_MS = 8000UL;
constexpr unsigned long SET_SETTINGS_PERIOD_MS = 12000UL;
constexpr unsigned long SET_PUMP_PERIOD_MS = 16000UL;
constexpr unsigned long SET_WATERING_PERIOD_MS = 20000UL;
constexpr unsigned long SET_LEDS_PERIOD_MS = 24000UL;

// Test polling
constexpr unsigned long TEST_POLLING_PERIOD_MS = 6000UL;

#endif