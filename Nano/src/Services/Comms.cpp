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

static unsigned long lastActivityMillis;

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

    lastActivityMillis = millis();

    clearBuffer();
}

// --------------------------------------------------
// Update
// --------------------------------------------------

void commsUpdate()
{
    while (espSerial.available())
    {
        lastActivityMillis = millis();

        char c = espSerial.read();

        if (c == '\n')
        {
            processMessage();
            continue;
        }

        if (c == '\r')
            continue;

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

    // Ignore ESP debug messages
    if (strncmp(rxBuffer, "ESP", 3) == 0)
    {
        clearBuffer();
        return;
    }

    Serial.print("NANO RX (");
    Serial.print(millis());
    Serial.print("): ");
    Serial.println(rxBuffer);

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
    Serial.print("NANO TX (");
    Serial.print(millis());
    Serial.print("): ");
    serializeJson(document, Serial);
    Serial.println();

    lastActivityMillis = millis();

    serializeJson(document, espSerial);
    espSerial.write('\n');

    lastActivityMillis = millis();
}


bool commsCanRunBlockingOperation()
{
    if (messageReady)
        return false;

    if (rxIndex != 0)
        return false;

    if (espSerial.available())
        return false;

    if (millis() - lastActivityMillis < COMMS_LED_GUARD_MS)
        return false;

    return true;
}