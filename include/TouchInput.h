#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include "DeckConfig.h"

enum class TouchEvent : uint8_t {
    NONE,
    DOWN,
    MOVE,
    UP
};

struct TouchSample {
    TouchEvent event;
    int16_t x;
    int16_t y;
    int16_t rawX;
    int16_t rawY;
};

class TouchInput {
public:
    TouchInput();
    void begin();
    TouchSample poll();

private:
    SPIClass _spi;
    XPT2046_Touchscreen _ts;
    bool _isPressed;
    uint32_t _lastChangeMs;
    int16_t _lastX;
    int16_t _lastY;
    int16_t _lastRawX;
    int16_t _lastRawY;
};
