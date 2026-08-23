#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <stdint.h>

void ledStripBegin();
void ledStripUpdate();

void getLedStripColor(uint8_t& hue, uint8_t& saturation, uint8_t& value);

bool ledStripIsLoop();

void ledStripSetLoop(bool loop);

void ledStripSetColor(uint8_t hue, uint8_t saturation, uint8_t value);

#endif