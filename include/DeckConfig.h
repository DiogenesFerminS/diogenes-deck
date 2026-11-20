#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

// Touch controller SPI pins for ESP32-2432S028R (CYD)
constexpr uint8_t TOUCH_IRQ_PIN  = 36;
constexpr uint8_t TOUCH_MOSI_PIN = 32;
constexpr uint8_t TOUCH_MISO_PIN = 39;
constexpr uint8_t TOUCH_CLK_PIN  = 25;
constexpr uint8_t TOUCH_CS_PIN   = 33;

// Display geometry in landscape mode (Rotation 1)
constexpr int16_t SCREEN_WIDTH  = 320;
constexpr int16_t SCREEN_HEIGHT = 240;
constexpr uint8_t SCREEN_ROTATION = 1;

// Raw XPT2046 calibration bounds (adjust if touch offset is observed)
constexpr int16_t TOUCH_MIN_X = 200;
constexpr int16_t TOUCH_MAX_X = 3700;
constexpr int16_t TOUCH_MIN_Y = 240;
constexpr int16_t TOUCH_MAX_Y = 3800;
constexpr uint32_t TOUCH_DEBOUNCE_MS  = 35;
constexpr int16_t  SWIPE_THRESHOLD_PX = 50;

// Grid layout configuration (2 pages, 2 rows x 3 columns = 6 buttons per page)
constexpr uint8_t PAGE_COUNT    = 2;
constexpr int16_t HEADER_HEIGHT = 32;
constexpr uint8_t GRID_COLS     = 3;
constexpr uint8_t GRID_ROWS     = 2;
constexpr uint8_t BUTTON_COUNT  = GRID_COLS * GRID_ROWS;
constexpr int16_t GRID_PADDING  = 10;
constexpr int16_t BUTTON_RADIUS = 10;

// Theme colors (RGB565)
constexpr uint16_t COLOR_BG          = TFT_BLACK; // Black background
constexpr uint16_t COLOR_HEADER_BG   = TFT_BLACK; // Black header background
constexpr uint16_t COLOR_CARD_BG     = 0x0841;    // Deepest dark gray card background
constexpr uint16_t COLOR_CARD_BORDER = 0x0134;    // Dark royal/navy blue border
constexpr uint16_t COLOR_TEXT        = TFT_WHITE; // Pure white text
constexpr uint16_t COLOR_BTN_PRESSED = TFT_WHITE; // White border highlight when pressed

// Bluetooth BLE HID configuration
constexpr const char* BLE_DEVICE_NAME = "Diogenes Deck";

struct DeckButton {
    uint8_t id;
    const char* label;
    const char* actionId;
    uint16_t accentColor;
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
};
