
#include <Arduino.h>

#include <config.h>
#include <utils.h>

#include "controllers/ESPAllOnPinManager.h"

#include "controllers/UI/ESPAllOnGUI.h"
#include "manager/ESPAllOn.h"
#include "mods/ESPinner_Stepper/StepperRunner.h"
#include <TickerFree.h>

#if MEMORYDEBUG
#include "manager/Memory_Manager.h"
#endif

/**
 * Main setup and loop functions for the ESPAllOn
 * project, which provides a web-based interface for configuring ESP32/ESP8266
 * pins and managing various hardware modules through the ESPUI library.
 */

// UI handles

/**
 * External action callback function for MQTT message sending
 * @param mode Action mode parameter
 */
void externalAction(uint16_t mode) { Serial.println("EXTERNAL ACTIOOOOON"); }

/**
 * External action callback function for blocked door handling
 * @param mode Action mode parameter
 */
void externalAction2(uint16_t mode) { Serial.println("Blocked Door"); }

String nameAction = "Send MQTT Message";
ESPAction ACTION(nameAction, "ACTION1", externalAction);
ESPAction ACTION2("BLocked Door", "ACTION2", externalAction2);

ESPALLON_Wifi wifi = ESPALLON_Wifi::getInstance();

/**
 * Setup initializes all system components
 *
 * Configures serial communication, WiFi connection, UI components,
 * pin management, and loads saved ESPinner configurations from storage.
 */
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

	ESPAllOnPinManager::getInstance();

	ESPAllOn::getInstance().setup();
	ESPAllOn::getInstance().begin();

	ESPinner_Manager::getInstance().loadFromStorage();
	UI_UpdateTicker.start();
	wifi.start();
	wifi.connectWifi();

#if MEMORYDEBUG
	Memory_Ticker.start();
#endif
}

/**
 * Main loop function - handles UI updates, WiFi management, and serial
 * Continuously updates the UI ticker, manages WiFi connection, and processes
 * serial commands for debugging and configuration purposes.
 */
void loop() {

	UI_UpdateTicker.update();
	wifi.update();

	// Execute all steppers in the StepperRunner (non-blocking)
	StepperRunner::getInstance().runAll();
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

#if MEMORYDEBUG
	Memory_Ticker.update();
#endif
}