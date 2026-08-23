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
constexpr unsigned long STATE_REQUEST_PERIOD_MS = 5000UL;
constexpr unsigned long SETTINGS_REQUEST_PERIOD_MS = 9000UL;

#endif