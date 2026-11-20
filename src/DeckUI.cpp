#include "DeckUI.h"

DeckUI::DeckUI()
    : _tft(),
      _currentPage(0),
      _buttons{
          {
              {0, "Terminal",    "CMD:TERMINAL",    COLOR_CARD_BORDER, 0, 0, 0, 0},
              {1, "Browser",     "CMD:BROWSER",     COLOR_CARD_BORDER, 0, 0, 0, 0},
              {2, "VS Code",     "CMD:CODE",        COLOR_CARD_BORDER, 0, 0, 0, 0},
              {3, "Opencode",    "CMD:OPENCODE",    COLOR_CARD_BORDER, 0, 0, 0, 0},
              {4, "Antigravity", "CMD:ANTIGRAVITY", COLOR_CARD_BORDER, 0, 0, 0, 0},
              {5, "Lock PC",     "CMD:LOCK",        COLOR_CARD_BORDER, 0, 0, 0, 0}
          },
          {
              {0, "Wake PC",     "CMD:WAKE_PC",     COLOR_CARD_BORDER, 0, 0, 0, 0},
              {1, "Suspend",     "CMD:SUSPEND",     COLOR_CARD_BORDER, 0, 0, 0, 0},
              {2, "Vol +",       "CMD:VOL_UP",      COLOR_CARD_BORDER, 0, 0, 0, 0},
              {3, "Vol -",       "CMD:VOL_DOWN",    COLOR_CARD_BORDER, 0, 0, 0, 0},
              {4, "Mute",        "CMD:MUTE",        COLOR_CARD_BORDER, 0, 0, 0, 0},
              {5, "Reboot",      "CMD:REBOOT",      COLOR_CARD_BORDER, 0, 0, 0, 0}
          }
      } {
    computeLayout();
}

void DeckUI::computeLayout() {
    const int16_t availW = SCREEN_WIDTH - (GRID_PADDING * (GRID_COLS + 1));
    const int16_t availH = (SCREEN_HEIGHT - HEADER_HEIGHT) - (GRID_PADDING * (GRID_ROWS + 1));
    const int16_t btnW   = availW / GRID_COLS;
    const int16_t btnH   = availH / GRID_ROWS;

    for (uint8_t page = 0; page < PAGE_COUNT; ++page) {
        for (uint8_t row = 0; row < GRID_ROWS; ++row) {
            for (uint8_t col = 0; col < GRID_COLS; ++col) {
                const uint8_t idx = row * GRID_COLS + col;
                _buttons[page][idx].x = GRID_PADDING + col * (btnW + GRID_PADDING);
                _buttons[page][idx].y = HEADER_HEIGHT + GRID_PADDING + row * (btnH + GRID_PADDING);
                _buttons[page][idx].w = btnW;
                _buttons[page][idx].h = btnH;
            }
        }
    }
}

void DeckUI::begin() {
    _tft.init();
    _tft.invertDisplay(true);
    _tft.setRotation(SCREEN_ROTATION);
    _tft.fillScreen(COLOR_BG);

    drawHeader("Page 1/2");
    drawCurrentPage();
}

void DeckUI::drawCurrentPage() {
    for (uint8_t i = 0; i < BUTTON_COUNT; ++i) {
        drawButton(i, false);
    }
}

void DeckUI::nextPage() {
    _currentPage = (_currentPage + 1) % PAGE_COUNT;
    char pageLabel[16];
    snprintf(pageLabel, sizeof(pageLabel), "Page %u/%u", _currentPage + 1, PAGE_COUNT);
    drawHeader(pageLabel);
    drawCurrentPage();
}

void DeckUI::prevPage() {
    _currentPage = (_currentPage == 0) ? (PAGE_COUNT - 1) : (_currentPage - 1);
    char pageLabel[16];
    snprintf(pageLabel, sizeof(pageLabel), "Page %u/%u", _currentPage + 1, PAGE_COUNT);
    drawHeader(pageLabel);
    drawCurrentPage();
}

uint8_t DeckUI::currentPage() const {
    return _currentPage;
}

void DeckUI::drawBoldString(const char* text, int16_t x, int16_t y, uint8_t datum) {
    _tft.setTextSize(1);
    _tft.setTextColor(COLOR_TEXT);
    _tft.setTextDatum(datum);
    _tft.drawString(text, x, y, 2);
    _tft.drawString(text, x + 1, y, 2);
}

void DeckUI::drawHeader(const char* statusText) {
    _tft.fillRect(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, COLOR_HEADER_BG);
    _tft.drawFastHLine(0, HEADER_HEIGHT - 1, SCREEN_WIDTH, COLOR_CARD_BORDER);

    drawBoldString("DIOGENES DECK", 10, HEADER_HEIGHT / 2, ML_DATUM);

    const int16_t dotCenterY = HEADER_HEIGHT / 2;
    const int16_t dotStartX  = 168;
    for (uint8_t p = 0; p < PAGE_COUNT; ++p) {
        const int16_t dotX = dotStartX + p * 14;
        if (p == _currentPage) {
            _tft.fillCircle(dotX, dotCenterY, 4, COLOR_TEXT);
        } else {
            _tft.drawCircle(dotX, dotCenterY, 4, COLOR_CARD_BORDER);
        }
    }

    drawBoldString(statusText, SCREEN_WIDTH - 10, HEADER_HEIGHT / 2, MR_DATUM);
}

void DeckUI::drawButton(uint8_t index, bool pressed) {
    if (index >= BUTTON_COUNT) {
        return;
    }

    const DeckButton& btn = _buttons[_currentPage][index];
    const uint16_t fill   = pressed ? COLOR_CARD_BORDER : COLOR_CARD_BG;
    const uint16_t border = pressed ? COLOR_BTN_PRESSED : COLOR_CARD_BORDER;

    _tft.fillRoundRect(btn.x, btn.y, btn.w, btn.h, BUTTON_RADIUS, fill);
    _tft.drawRoundRect(btn.x, btn.y, btn.w, btn.h, BUTTON_RADIUS, border);
    _tft.drawRoundRect(btn.x + 1, btn.y + 1, btn.w - 2, btn.h - 2, BUTTON_RADIUS - 1, border);

    drawBoldString(btn.label, btn.x + (btn.w / 2), btn.y + (btn.h / 2), MC_DATUM);
}

int8_t DeckUI::hitTest(int16_t x, int16_t y) const {
    for (uint8_t i = 0; i < BUTTON_COUNT; ++i) {
        const DeckButton& btn = _buttons[_currentPage][i];
        if (x >= btn.x && x < (btn.x + btn.w) &&
            y >= btn.y && y < (btn.y + btn.h)) {
            return static_cast<int8_t>(i);
        }
    }
    return -1;
}

const DeckButton& DeckUI::getButton(uint8_t index) const {
    return _buttons[_currentPage][index];
}
