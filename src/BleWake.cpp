#include "BleWake.h"
#include "DeckConfig.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLEHIDDevice.h>
#include <HIDTypes.h>

namespace {
const uint8_t HID_KEYBOARD_REPORT_MAP[] = {
    USAGE_PAGE(1),      0x01,       
    USAGE(1),           0x06,      
    COLLECTION(1),      0x01,       
    REPORT_ID(1),       0x01,       
    USAGE_PAGE(1),      0x07,      
    USAGE_MINIMUM(1),   0xE0,      
    USAGE_MAXIMUM(1),   0xE7,       
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01,
    REPORT_SIZE(1),     0x01,       
    REPORT_COUNT(1),    0x08,
    HIDINPUT(1),        0x02,       
    REPORT_COUNT(1),    0x01,       
    REPORT_SIZE(1),     0x08,
    HIDINPUT(1),        0x01,       
    REPORT_COUNT(1),    0x06,       
    REPORT_SIZE(1),     0x08,
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x65,       
    USAGE_PAGE(1),      0x07,
    USAGE_MINIMUM(1),   0x00,
    USAGE_MAXIMUM(1),   0x65,
    HIDINPUT(1),        0x00,      
    END_COLLECTION(0)
};

BLEHIDDevice* hidDevice = nullptr;
BLECharacteristic* inputKeyboard = nullptr;
volatile bool bleClientConnected = false;

class DeckBleCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* /*pServer*/) override {
        bleClientConnected = true;
        if (inputKeyboard != nullptr) {
            BLE2902* desc = static_cast<BLE2902*>(
                inputKeyboard->getDescriptorByUUID(BLEUUID(static_cast<uint16_t>(0x2902)))
            );
            if (desc != nullptr) {
                desc->setNotifications(true);
            }
        }
    }

    void onDisconnect(BLEServer* pServer) override {
        bleClientConnected = false;
        pServer->getAdvertising()->start();
    }
};
}

BleWake::BleWake() : _connected(false) {}

void BleWake::begin() {
    BLEDevice::init(BLE_DEVICE_NAME);
    BLEServer* server = BLEDevice::createServer();
    server->setCallbacks(new DeckBleCallbacks());

    hidDevice = new BLEHIDDevice(server);
    inputKeyboard = hidDevice->inputReport(1);

    hidDevice->manufacturer()->setValue("Diogenes");
    hidDevice->pnp(0x02, 0xe502, 0xa111, 0x0210);
    hidDevice->hidInfo(0x00, 0x01);

    BLESecurity* security = new BLESecurity();
    security->setAuthenticationMode(ESP_LE_AUTH_BOND);

    hidDevice->reportMap(const_cast<uint8_t*>(HID_KEYBOARD_REPORT_MAP), sizeof(HID_KEYBOARD_REPORT_MAP));
    hidDevice->startServices();

    BLEAdvertising* advertising = server->getAdvertising();
    advertising->setAppearance(HID_KEYBOARD);
    advertising->addServiceUUID(hidDevice->hidService()->getUUID());
    advertising->start();
    hidDevice->setBatteryLevel(100);
}

bool BleWake::isConnected() const {
    return bleClientConnected;
}

bool BleWake::sendWakeKey() {
    if (!bleClientConnected || inputKeyboard == nullptr) {
        return false;
    }

    uint8_t pressReport[8]   = {0x00, 0x00, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t releaseReport[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    inputKeyboard->setValue(pressReport, sizeof(pressReport));
    inputKeyboard->notify();
    delay(20);
    inputKeyboard->setValue(releaseReport, sizeof(releaseReport));
    inputKeyboard->notify();
    return true;
}
