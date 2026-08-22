#include <Arduino.h>

#include "Config.h"
#include "Pins.h"
#include "Types.h"

#include "IO/Inputs.h"
#include "IO/Outputs.h"
#include "IO/LEDStrip.h"

#include "Sensors/DayNight.h"
#include "Sensors/Environment.h"

#include "Control/Pump.h"
#include "Control/Watering.h"
#include "Control/Refill.h"

#include "Services/Scheduler.h"
#include "Services/Settings.h"
#include "Services/EEPROMStorage.h"
#include "Services/Comms.h"
#include "Services/CommandHandler.h"

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println("Nano started");

    outputsBegin();
    inputsBegin();

    settingsBegin();    // Load factory defaults
    eepromLoad();       // Replace with saved settings if available

    schedulerBegin();
    dayNightBegin();
    environmentBegin();

    pumpBegin();
    wateringBegin();
    refillBegin();
    ledStripBegin();

    commsBegin();
    commandHandlerBegin();
}

void loop() {
    inputsUpdate();

    schedulerUpdate();
    dayNightUpdate();
    environmentUpdate();
    
    pumpUpdate();
    wateringUpdate();
    refillUpdate();
    ledStripUpdate();

    commsUpdate();
    commandHandlerUpdate();
}