#include "IO/LEDStrip.h"

#include <FastLED.h>

#include "Pins.h"
#include "Config.h"
#include "Services/Settings.h"
#include "Sensors/DayNight.h"

static CRGB leds[LED_COUNT];

static uint8_t loopHue = 0;

static bool stripEnabled = false;

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
// Update strip output
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

    FastLED.show();
}

// --------------------------------------------------
// Begin
// --------------------------------------------------

void ledStripBegin()
{
    FastLED.addLeds<WS2812B, PIN_LEDSTRIP, GRB>(leds, LED_COUNT);

    FastLED.clear();
    FastLED.show();
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
            FastLED.show();

            stripEnabled = false;
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

        // Start LOOP from configured hue.
        if (settings.leds.loopMode)
            loopHue = settings.leds.hue;

        updateStripColor();
        return;
    }

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
    else
    {
        updateStripColor();
    }
}

// --------------------------------------------------
// Get current color
// --------------------------------------------------
void getLedStripColor(
    uint8_t& outHue,
    uint8_t& outSaturation,
    uint8_t& outValue)
{
    if (settings.leds.loopMode)
        outHue = loopHue;
    else
        outHue = settings.leds.hue;

    outSaturation = settings.leds.saturation;
    outValue = settings.leds.value;
}

// --------------------------------------------------
// Get mode
// --------------------------------------------------
bool ledStripIsLoop()
{
    return settings.leds.loopMode;
}