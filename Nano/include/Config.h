#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Serial
constexpr uint32_t SERIAL_BAUD_RATE = 115200UL;

// Pump
constexpr uint16_t PUMP_PERIOD_MIN = 30;
constexpr uint16_t PUMP_PERIOD_MAX = 1440;

constexpr byte PUMP_RUNTIME_MIN = 1;
constexpr byte PUMP_RUNTIME_MAX = 60;

// Watering
constexpr byte WATER_HOUR_MIN = 0;
constexpr byte WATER_HOUR_MAX = 23;

constexpr byte WATER_MINUTE_MIN = 0;
constexpr byte WATER_MINUTE_MAX = 45;
constexpr byte WATER_MINUTE_STEP = 15;

constexpr byte WATER_PERIOD_MIN = 1;
constexpr byte WATER_PERIOD_MAX = 30;

constexpr byte WATER_RUNTIME_MIN = 1;
constexpr byte WATER_RUNTIME_MAX = 60;

// LDR (0..1023)
constexpr uint16_t LDR_DAY_THRESHOLD   = 600;
constexpr uint16_t LDR_NIGHT_THRESHOLD = 200;

// LED strip
constexpr byte LED_COUNT = 4;
constexpr uint16_t LED_RATE_MIN_MS = 50;
constexpr uint16_t LED_RATE_MAX_MS = 500;

// DHT22
constexpr byte ENVIRONMENT_PERIOD_S = 5;

// Comms
constexpr uint32_t COMMS_BAUD_RATE = 9600UL;
constexpr uint16_t COMMS_BUFFER_SIZE = 192;
constexpr byte COMMS_COMMAND_SIZE = 32;
constexpr uint16_t COMMS_CRC_INITIAL = 0xFFFF;
constexpr uint16_t COMMS_TIMEOUT_MS = 2000;
constexpr uint16_t COMMS_LED_GUARD_MS = 2;

#endif