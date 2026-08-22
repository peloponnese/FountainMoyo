#include "Services/Comms.h"

#include "Config.h"

static char rxBuffer[COMMS_BUFFER_SIZE];
static uint16_t rxIndex = 0;
static bool messageReady = false;

static JsonDocument messageDocument;

static void processMessage();
static void clearBuffer();

// --------------------------------------------------
// Begin
// --------------------------------------------------
void commsBegin()
{
    Serial.begin(SERIAL_BAUD_RATE);

    rxIndex = 0;
    messageReady = false;

    clearBuffer();
}

// --------------------------------------------------
// Update
// --------------------------------------------------
void commsUpdate()
{
    while (Serial.available())
    {
        char c = Serial.read();

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

    Serial.print("ESP RX: ");
    Serial.println(rxBuffer);

    messageDocument.clear();

    DeserializationError error = deserializeJson(messageDocument, rxBuffer);

    if (error)
    {
        Serial.print("ESP JSON ERROR: ");
        Serial.println(error.c_str());

        clearBuffer();
        return;
    }

    Serial.println("ESP JSON OK");

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
// Clear message
// --------------------------------------------------
void commsClearMessage()
{
    messageDocument.clear();
    messageReady = false;
}

// --------------------------------------------------
// Send JSON
// --------------------------------------------------
void commsSend(JsonDocument& document)
{
    serializeJson(document, Serial);
    Serial.write('\n');
}