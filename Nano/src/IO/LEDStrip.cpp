#include "IO/LEDStrip.h"

#include <FastLED.h>

#include "Pins.h"
#include "Config.h"
#include "Services/Settings.h"
#include "Sensors/DayNight.h"

static CRGB leds[LED_COUNT];

static uint8_t loopHue = 0;

static bool stripEnabled = false;

static bool outputPending = false;

static unsigned long previousMillis = 0;

// --------------------------------------------------
// LED rate -> period
// --------------------------------------------------

static uint16_t ledRateToPeriod(uint8_t rate)
{
    return LED_RATE_MAX_MS -
        ((uint32_t)rate *
        (LED_RATE_MAX_MS - LED_RATE_MIN_MS)) / 255;
}

// --------------------------------------------------
// Prepare strip color
// --------------------------------------------------

static void updateStripColor()
{
    fill_solid(
        leds,
        LED_COUNT,
        CHSV(
            settings.leds.loopMode ? loopHue : settings.leds.hue,
            settings.leds.saturation,
            settings.leds.value));

    outputPending = true;
}

// --------------------------------------------------
// Begin
// --------------------------------------------------

void ledStripBegin()
{
    FastLED.addLeds<WS2812B, PIN_LEDSTRIP, GRB>(leds, LED_COUNT);

    FastLED.clear();
    FastLED.show();

    outputPending = false;
}

// --------------------------------------------------
// Update
// --------------------------------------------------

void ledStripUpdate()
{
    // --------------------------------------------------
    // DAY or disabled
    // --------------------------------------------------

    if (getDayState() == DAY || !settings.leds.enabled)
    {
        if (stripEnabled)
        {
            FastLED.clear();

            stripEnabled = false;
            outputPending = true;
        }

        return;
    }

    // --------------------------------------------------
    // Night and enabled
    // --------------------------------------------------

    if (!stripEnabled)
    {
        stripEnabled = true;

        previousMillis = millis();

        FastLED.setBrightness(255);

        if (settings.leds.loopMode)
            loopHue = settings.leds.hue;

        updateStripColor();

        return;
    }

    // --------------------------------------------------
    // Loop mode
    // --------------------------------------------------

    if (settings.leds.loopMode)
    {
        const uint16_t period = ledRateToPeriod(settings.leds.rate);
        const unsigned long now = millis();

        if (now - previousMillis < period)
            return;

        previousMillis = now;

        loopHue++;

        updateStripColor();
    }
}

// --------------------------------------------------
// Output
// --------------------------------------------------

void ledStripOutput()
{
    if (!outputPending)
        return;

    FastLED.show();

    outputPending = false;
}

// --------------------------------------------------
// Get current color
// --------------------------------------------------

uint8_t getLedStripHue()
{
    uint8_t outHue = 0;

    if (settings.leds.loopMode)
        outHue = loopHue;
    else
        outHue = settings.leds.hue;

    return outHue;
}