#include <Arduino.h>
#include "BleWake.h"
#include "DeckConfig.h"
#include "DeckUI.h"
#include "TouchInput.h"

namespace {
DeckUI ui;
TouchInput touch;
BleWake bleWake;
int8_t activeButton = -1;
int16_t touchStartX = 0;
bool isSwiping = false;
char serialBuf[48];
uint8_t serialPos = 0;

void pollSerialStatus() {
    while (Serial.available() > 0) {
        const char c = static_cast<char>(Serial.read());
        if (c == '\n' || c == '\r') {
            if (serialPos > 0) {
                serialBuf[serialPos] = '\0';
                if (strncmp(serialBuf, "ACK:", 4) == 0) {
                    ui.drawHeader(serialBuf + 4);
                }
                serialPos = 0;
            }
        } else if (serialPos < sizeof(serialBuf) - 1) {
            serialBuf[serialPos++] = c;
        }
    }
}
}

void setup() {
    Serial.begin(115200);
    ui.begin();
    touch.begin();
    bleWake.begin();
}

void loop() {
    pollSerialStatus();
    const TouchSample sample = touch.poll();

    switch (sample.event) {
        case TouchEvent::DOWN: {
            touchStartX = sample.x;
            isSwiping = false;
            const int8_t hit = ui.hitTest(sample.x, sample.y);
            if (hit >= 0) {
                activeButton = hit;
                ui.drawButton(static_cast<uint8_t>(activeButton), true);
            }
            Serial.printf("[TOUCH] raw=(%d,%d) px=(%d,%d) btn=%d\n",
                          sample.rawX, sample.rawY, sample.x, sample.y, hit);
            break;
        }

        case TouchEvent::MOVE: {
            const int16_t dx = sample.x - touchStartX;
            if (abs(dx) >= SWIPE_THRESHOLD_PX) {
                isSwiping = true;
                if (activeButton >= 0) {
                    ui.drawButton(static_cast<uint8_t>(activeButton), false);
                    activeButton = -1;
                }
            } else if (activeButton >= 0) {
                const int8_t currentHit = ui.hitTest(sample.x, sample.y);
                if (currentHit != activeButton) {
                    ui.drawButton(static_cast<uint8_t>(activeButton), false);
                    activeButton = -1;
                }
            }
            break;
        }

        case TouchEvent::UP: {
            const int16_t dx = sample.x - touchStartX;
            if (dx <= -SWIPE_THRESHOLD_PX) {
                if (activeButton >= 0) {
                    ui.drawButton(static_cast<uint8_t>(activeButton), false);
                    activeButton = -1;
                }
                ui.nextPage();
                Serial.printf("[SWIPE] Left -> Page %u\n", ui.currentPage() + 1);
            } else if (dx >= SWIPE_THRESHOLD_PX) {
                if (activeButton >= 0) {
                    ui.drawButton(static_cast<uint8_t>(activeButton), false);
                    activeButton = -1;
                }
                ui.prevPage();
                Serial.printf("[SWIPE] Right -> Page %u\n", ui.currentPage() + 1);
            } else if (!isSwiping && activeButton >= 0) {
                const DeckButton& btn = ui.getButton(static_cast<uint8_t>(activeButton));
                ui.drawButton(static_cast<uint8_t>(activeButton), false);
                ui.drawHeader(btn.label);
                Serial.println(btn.actionId);

                if (strcmp(btn.actionId, "CMD:WAKE_PC") == 0) {
                    if (bleWake.sendWakeKey()) {
                        ui.drawHeader("BLE Wake Sent");
                    } else {
                        ui.drawHeader("Pair BLE first");
                    }
                }
                activeButton = -1;
            }
            isSwiping = false;
            break;
        }

        case TouchEvent::NONE:
        default:
            break;
    }
}