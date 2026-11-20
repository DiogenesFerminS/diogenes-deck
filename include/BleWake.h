#pragma once

#include <Arduino.h>

class BleWake {
public:
    BleWake();
    void begin();
    bool isConnected() const;
    bool sendWakeKey();

private:
    bool _connected;
};
