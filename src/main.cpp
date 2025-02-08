
#include <Arduino.h>

#include <config.h>
#include <utils.h>

#include "controllers/ESPAllOnPinManager.h"
#include "controllers/Wifi_Controller.h"

#include "controllers/UI/ESPAllOnGUI.h"
#include "manager/ESPAllOn.h"
#include <TickerFree.h>

#include "controllers/ESPAllOn_Wifi.h"

// UI handles

void externalAction(uint16_t mode) { Serial.println("EXTERNAL ACTIOOOOON"); }
void externalAction2(uint16_t mode) { Serial.println("Blocked Door"); }

String nameAction = "Send MQTT Message";
ESPAction ACTION(nameAction, "ACTION1", externalAction);
ESPAction ACTION2("BLocked Door", "ACTION2", externalAction2);

ESPALLON_Wifi wifi = ESPALLON_Wifi::getInstance();

void setup() {
	randomSeed(0);
	Serial.begin(115200);
	while (!Serial)
		;

	wifi.connectWifi();
#if defined(ESP32)
	WiFi.setSleep(false); // For the ESP32: turn off sleeping to increase UI
						  // responsivness (at the cost of power use)
#endif
	ESPAllOn::getInstance().addAction(ACTION);
	ESPAllOn::getInstance().addAction(ACTION2);

	ESPAllOn::getInstance().setup();
	ESPAllOn::getInstance().begin();
	ESPAllOnPinManager::getInstance();
	ESPAllOnPinManager::getInstance().printLabels();
	ESPinner_Manager::getInstance().loadFromStorage();
	UI_UpdateTicker.start();
	wifi.start();
	wifi.connectWifi();
}

void loop() {

	UI_UpdateTicker.update();
	wifi.update();
	if (Serial.available()) {
		switch (Serial.read()) {
		case 'w': // Print IP details
			Serial.println(WiFi.localIP());
			break;
		case 'W': // Reconnect wifi
			wifi.connectWifi();
			break;
		case 'C': // Force a crash (for testing exception decoder)
#if !defined(ESP32)
			((void (*)())0xf00fdead)();
#endif
			break;
		case 'O': // Force a crash (for testing exception decoder)
			DUMP_PINOUT();
			break;
		default:
			DUMP_PINOUT();
			break;
		}
	}

#if !defined(ESP32)
	// We don't need to call this explicitly on ESP32 but we do on 8266
	MDNS.update();
#endif
}