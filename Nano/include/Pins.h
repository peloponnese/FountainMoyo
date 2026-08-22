#ifndef PINS_H
#define PINS_H

// Outputs
const byte PIN_PUMP      = 2;
const byte PIN_VALVE1    = 3;
const byte PIN_VALVE2    = 4;
const byte PIN_LEDSTRIP  = 5;

// Inputs
const byte PIN_SW_PUMP   = 6;
const byte PIN_SW_WATER  = 7;
const byte PIN_REED_HIGH = 8;
const byte PIN_REED_LOW  = 9;

// Status LEDs
const byte PIN_LED_PUMP     = 10;
const byte PIN_LED_VALVE1   = 11;
const byte PIN_LED_VALVE2   = 12;
const byte PIN_LED_EMPTY    = 13;

// ESP-01S (SoftwareSerial)
const byte PIN_ESP_RX = A4;
const byte PIN_ESP_TX = A5;

// Analog
const byte PIN_LDR = A0;

// DHT22
const byte PIN_DHT = A3;

#endif