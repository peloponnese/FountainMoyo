#include "IO/LEDStrip.h"

#include <FastLED.h>

#include "Pins.h"
#include "Config.h"
#include "Services/Settings.h"
#include "Sensors/DayNight.h"

static CRGB leds[LED_COUNT];

static byte hue = 0;

static bool stripEnabled = false;

static unsigned long previousMillis = 0;

static uint16_t ledRateToPeriod(byte rate)
{
    return LED_RATE_MAX_MS -
        ((uint32_t)rate *
        (LED_RATE_MAX_MS - LED_RATE_MIN_MS)) / 255;
}

void ledStripBegin()
{
    FastLED.addLeds<WS2812B, PIN_LEDSTRIP, GRB>(leds, LED_COUNT);

    FastLED.clear();
    FastLED.show();
}

void ledStripUpdate()
{
    if (getDayState() == DAY || !settings.led.enabled)
    {
        if (stripEnabled)
        {
            FastLED.clear();
            FastLED.show();
            
            stripEnabled = false;
        }

        return;
    }

    if (!stripEnabled)
    {
        stripEnabled = true;

        previousMillis = millis();

        FastLED.setBrightness(settings.led.brightness);

        fill_solid(
            leds,
            LED_COUNT,
            CHSV(hue, 255, 255));

        FastLED.show();

        return;
    }

    FastLED.setBrightness(settings.led.brightness);

    const uint16_t period = ledRateToPeriod(settings.led.rate);

    const unsigned long now = millis();

    if (now - previousMillis < period)
        return;

    previousMillis = now;

    hue++;

    fill_solid(
        leds,
        LED_COUNT,
        CHSV(hue, 255, 255));

    FastLED.show();
}