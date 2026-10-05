#include "Services/CommandLoader.h"

#include "Web/WebSettings.h"
#include "Web/WebControl.h"

// --------------------------------------------------
// Pending command
// --------------------------------------------------

static JsonDocument document;
static bool commandPending = false;

// --------------------------------------------------
// Internal helper
// --------------------------------------------------

static void prepareCommand(uint16_t id, const char* command)
{
    document.clear();

    document["id"] = id;
    document["cmd"] = command;

    commandPending = true;
}

// --------------------------------------------------
// GET_STATE
// --------------------------------------------------

void loadCommandGetState(uint16_t id)
{
    prepareCommand(id, "get_state");
}

// --------------------------------------------------
// GET_SETTINGS
// --------------------------------------------------

void loadCommandGetSettings(uint16_t id)
{
    prepareCommand(id, "get_settings");
}

// --------------------------------------------------
// SET_SETTINGS_PUMP
// --------------------------------------------------

void loadCommandSetSettingsPump(uint16_t id)
{
    prepareCommand(id, "set_settings_pump");

    document["dayPeriod"] = webSettings.pump.dayPeriod;
    document["dayRuntime"] = webSettings.pump.dayRuntime;
    document["nightPeriod"] = webSettings.pump.nightPeriod;
    document["nightRuntime"] = webSettings.pump.nightRuntime;
}

// --------------------------------------------------
// SET_SETTINGS_WATERING
// --------------------------------------------------

void loadCommandSetSettingsWatering(uint16_t id)
{
    prepareCommand(id, "set_settings_watering");

    document["hour"] = webSettings.watering.hour;
    document["minute"] = webSettings.watering.minute;
    document["periodDays"] = webSettings.watering.periodDays;
    document["runtime"] = webSettings.watering.runtime;
}

// --------------------------------------------------
// SET_SETTINGS_LEDS
// --------------------------------------------------

void loadCommandSetSettingsLeds(uint16_t id)
{
    prepareCommand(id, "set_settings_leds");

    document["enabled"] = webSettings.leds.enabled;
    document["loop"] = webSettings.leds.loop;
    document["rate"] = webSettings.leds.rate;
    document["H"] = webSettings.leds.hue;
    document["S"] = webSettings.leds.saturation;
    document["V"] = webSettings.leds.value;
}

// --------------------------------------------------
// SET_PUMP
// --------------------------------------------------

void loadCommandSetPump(uint16_t id)
{
    prepareCommand(id, "set_pump");

    document["enabled"] = webControl.pump.manualOperation;
}

// --------------------------------------------------
// SET_WATERING
// --------------------------------------------------

void loadCommandSetWatering(uint16_t id)
{
    prepareCommand(id, "set_watering");

    document["enabled"] = webControl.watering.manualOperation;
}

// --------------------------------------------------
// SET_LEDS
// --------------------------------------------------

void loadCommandSetLeds(uint16_t id)
{
    prepareCommand(id, "set_leds");

    document["loop"] = !webSettings.leds.loop;
    document["hue"] = webSettings.leds.hue;
}

// --------------------------------------------------
// ACK
// --------------------------------------------------

void loadCommandAck(uint16_t id)
{
    prepareCommand(id, "ack");
}

// --------------------------------------------------
// Pending command
// --------------------------------------------------

bool commandHasReply()
{
    return commandPending;
}

JsonDocument& commandGetReply()
{
    return document;
}

void commandClearReply()
{
    document.clear();
    commandPending = false;
}