#include "TouchInput.h"

TouchInput::TouchInput()
    : _spi(VSPI),
      _ts(TOUCH_CS_PIN, TOUCH_IRQ_PIN),
      _isPressed(false),
      _lastChangeMs(0),
      _lastX(0),
      _lastY(0),
      _lastRawX(0),
      _lastRawY(0) {}

void TouchInput::begin() {
    _spi.begin(TOUCH_CLK_PIN, TOUCH_MISO_PIN, TOUCH_MOSI_PIN, TOUCH_CS_PIN);
    _ts.begin(_spi);
    _ts.setRotation(SCREEN_ROTATION);
}

TouchSample TouchInput::poll() {
    const uint32_t now = millis();
    const bool rawTouched = _ts.touched();

    if (rawTouched) {
        TS_Point p = _ts.getPoint();

        int16_t mappedX = map(p.x, TOUCH_MIN_X, TOUCH_MAX_X, 0, SCREEN_WIDTH - 1);
        int16_t mappedY = map(p.y, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, SCREEN_HEIGHT - 1);

        _lastX = constrain(mappedX, 0, SCREEN_WIDTH - 1);
        _lastY = constrain(mappedY, 0, SCREEN_HEIGHT - 1);
        _lastRawX = p.x;
        _lastRawY = p.y;
        _lastChangeMs = now;

        if (!_isPressed) {
            _isPressed = true;
            return {TouchEvent::DOWN, _lastX, _lastY, _lastRawX, _lastRawY};
        }
        return {TouchEvent::MOVE, _lastX, _lastY, _lastRawX, _lastRawY};
    }

    if (_isPressed && (now - _lastChangeMs >= TOUCH_DEBOUNCE_MS)) {
        _isPressed = false;
        return {TouchEvent::UP, _lastX, _lastY, _lastRawX, _lastRawY};
    }

    return {TouchEvent::NONE, _lastX, _lastY, _lastRawX, _lastRawY};
}
