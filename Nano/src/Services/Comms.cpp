#include "Services/Comms.h"

#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#include "Config.h"
#include "Pins.h"

static SoftwareSerial espSerial(PIN_ESP_RX, PIN_ESP_TX);

static char rxBuffer[COMMS_BUFFER_SIZE];
static uint16_t rxIndex;

static bool messageReady;
static uint16_t messageId;
static char command[COMMS_COMMAND_SIZE];

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
    messageId = 0;
    command[0] = '\0';

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
            commsSendCommError();
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

    messageDocument.clear();

    Serial.println(rxBuffer);

    // --------------------------------------------------
    // Parse JSON
    // --------------------------------------------------

    DeserializationError error = deserializeJson(messageDocument, rxBuffer);

    if (error)
    {
        Serial.println("Failed to parse JSON");

        commsSendCommError();
        clearBuffer();

        return;
    }

    // --------------------------------------------------
    // ID
    // --------------------------------------------------

    if (!messageDocument["id"].is<uint16_t>())
    {
        commsSendCommError();
        clearBuffer();

        return;
    }

    messageId = messageDocument["id"].as<uint16_t>();

    // --------------------------------------------------
    // Command
    // --------------------------------------------------

    if (!messageDocument["cmd"].is<const char*>())
    {
        commsSendCommError();
        clearBuffer();

        return;
    }

    const char* receivedCommand =
        messageDocument["cmd"].as<const char*>();

    strncpy(
        command,
        receivedCommand,
        COMMS_COMMAND_SIZE - 1
    );

    command[COMMS_COMMAND_SIZE - 1] = '\0';

    // --------------------------------------------------
    // Remove protocol field
    // --------------------------------------------------

    messageDocument.remove("crc");

    // --------------------------------------------------
    // Message ready
    // --------------------------------------------------

    messageReady = true;

    clearBuffer();
}

// --------------------------------------------------
// Clear receive buffer
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
    command[0] = '\0';
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
// Get command
// --------------------------------------------------

const char* commsGetCommand()
{
    return command;
}

// --------------------------------------------------
// Get ID
// --------------------------------------------------

uint16_t commsGetId()
{
    return messageId;
}

// --------------------------------------------------
// Get integer
// --------------------------------------------------

bool commsGetInt(const char* key, int& value)
{
    if (!messageDocument[key].is<int>())
        return false;

    value = messageDocument[key].as<int>();

    return true;
}

// --------------------------------------------------
// Get byte
// --------------------------------------------------

bool commsGetByte(const char* key, byte& value)
{
    if (!messageDocument[key].is<int>())
        return false;

    int temp = messageDocument[key].as<int>();

    if (temp < 0 || temp > 255)
        return false;

    value = (byte)temp;

    return true;
}

// --------------------------------------------------
// Get bool
// --------------------------------------------------

bool commsGetBool(const char* key, bool& value)
{
    if (!messageDocument[key].is<bool>())
        return false;

    value = messageDocument[key].as<bool>();

    return true;
}

// --------------------------------------------------
// Send JSON
// --------------------------------------------------

void commsSendJson(JsonDocument& document)
{
    serializeJson(document, espSerial);
    espSerial.write('\n');
}

// --------------------------------------------------
// Debug JSON
// --------------------------------------------------

void commsDebugJson(JsonDocument& document)
{
    Serial.println("Sending JSON:");

    serializeJson(document, Serial);

    Serial.println();
}

// --------------------------------------------------
// Send OK
// --------------------------------------------------

void commsSendOk(uint16_t id)
{
    JsonDocument document;

    document["id"] = id;
    document["cmd"] = "ok";

    commsSendJson(document);
}

// --------------------------------------------------
// Communication error
// --------------------------------------------------

void commsSendCommError()
{
    JsonDocument document;

    document["cmd"] = "comm_error";

    commsSendJson(document);
}

// --------------------------------------------------
// Unknown command
// --------------------------------------------------

void commsSendUnknownCommand(uint16_t id)
{
    JsonDocument document;

    document["id"] = id;
    document["cmd"] = "unknown_command";

    commsSendJson(document);
}