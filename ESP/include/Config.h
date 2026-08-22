#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Serial
constexpr uint32_t SERIAL_BAUD_RATE = 9600UL;

// Nano communication
constexpr uint16_t COMMS_BUFFER_SIZE = 192;
constexpr unsigned long REQUEST_TIMEOUT_MS = 10000UL;
constexpr byte NANO_MAX_RETRIES = 10;

// Periodic environment request
constexpr unsigned long ENVIRONMENT_PERIOD_MS = 5000UL;

#endif