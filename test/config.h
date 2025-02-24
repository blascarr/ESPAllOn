#ifndef _ESP_CONFIG_H
#define _ESP_CONFIG_H

#include <Arduino.h>
#include <ESPUI.h>
#include <unity.h>

#include <../src/config.h>
#include <../src/utils.h>

IPAddress LOCAL_PC(192, 168, 1, 130);
uint16_t LOCAL_PC_PORT = 3000;

uint8_t esp_mac[] = {0x60, 0x8B, 0x0E, 0x01, 0x5A, 0x32};

// ----------- SERVER CONFIG ----------- //
#define SERVER_PORT 80
#define HOST_PORT 80

#include "../src/controllers/Wifi_Controller.h"

#include "../src/models/PinSerializable.h"

#include "../src/controllers/ESPAllOnPinManager.h"
#include "../src/controllers/UI/ESPAllOnGUI.h"

#include "../src/manager/ESPAllOn.h"
#include <TickerFree.h>

#endif