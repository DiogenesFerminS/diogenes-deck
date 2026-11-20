#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "DeckConfig.h"

class DeckUI {
public:
    DeckUI();
    void begin();
    void drawHeader(const char* statusText);
    void drawButton(uint8_t index, bool pressed);
    void drawCurrentPage();
    void nextPage();
    void prevPage();
    uint8_t currentPage() const;
    int8_t hitTest(int16_t x, int16_t y) const;
    const DeckButton& getButton(uint8_t index) const;

private:
    TFT_eSPI _tft;
    uint8_t _currentPage;
    DeckButton _buttons[PAGE_COUNT][BUTTON_COUNT];
    void computeLayout();
    void drawBoldString(const char* text, int16_t x, int16_t y, uint8_t datum);
};
