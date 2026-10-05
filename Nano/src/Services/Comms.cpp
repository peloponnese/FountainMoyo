#include "Services/Comms.h"

#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#include "Config.h"
#include "Pins.h"

static SoftwareSerial espSerial(PIN_ESP_RX, PIN_ESP_TX);

static char rxBuffer[COMMS_BUFFER_SIZE];
static uint16_t rxIndex;

static bool messageReady;

static JsonDocument messageDocument;

// --------------------------------------------------
// Internal functions
// --------------------------------------------------

static void processMessage();
static void clearBuffer();

// --------------------------------------------------
// Begin
// --------------------------------------------------

void commsBegin()
{
    espSerial.begin(COMMS_BAUD_RATE);

    rxIndex = 0;
    messageReady = false;

    clearBuffer();
}

// --------------------------------------------------
// Update
// --------------------------------------------------

void commsUpdate()
{
    while (espSerial.available())
    {
        char c = espSerial.read();

        // End of message
        if (c == '\n')
        {
            processMessage();
            continue;
        }

        // Ignore carriage return
        if (c == '\r')
            continue;

        // Buffer overflow
        if (rxIndex >= COMMS_BUFFER_SIZE - 1)
        {
            clearBuffer();
            continue;
        }

        rxBuffer[rxIndex] = c;
        rxIndex++;

        rxBuffer[rxIndex] = '\0';
    }
}

// --------------------------------------------------
// Process message
// --------------------------------------------------
static void processMessage()
{
    if (rxIndex == 0)
    {
        clearBuffer();
        return;
    }

    Serial.print("NANO RX: ");
    Serial.println(rxBuffer);

    // Ignore ESP debug messages
    if (strncmp(rxBuffer, "ESP", 3) == 0)
    {
        clearBuffer();
        return;
    }

    messageDocument.clear();

    DeserializationError error = deserializeJson(messageDocument, rxBuffer);

    if (error)
    {
        clearBuffer();
        return;
    }

    messageReady = true;

    clearBuffer();
}

// --------------------------------------------------
// Clear RX buffer
// --------------------------------------------------
static void clearBuffer()
{
    rxIndex = 0;
    rxBuffer[0] = '\0';
}

// --------------------------------------------------
// Clear message
// --------------------------------------------------
void commsClearMessage()
{
    messageDocument.clear();
    messageReady = false;
}

// --------------------------------------------------
// Message status
// --------------------------------------------------
bool commsHasMessage()
{
    return messageReady;
}

// --------------------------------------------------
// Get message
// --------------------------------------------------
JsonDocument& commsGetMessage()
{
    return messageDocument;
}

// --------------------------------------------------
// Send JSON
// --------------------------------------------------
void commsSend(JsonDocument& document)
{
    Serial.print("NANO TX: ");
    serializeJson(document, Serial);
    Serial.println();

    serializeJson(document, espSerial);
    espSerial.write('\n');
}