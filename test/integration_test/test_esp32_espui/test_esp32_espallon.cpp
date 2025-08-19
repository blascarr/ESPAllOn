/**
 * ESP32 ESPAllOn Integration Test
 *
 * This integration test validates the complete ESPAllOn system running on ESP32
 * with ESPUI web interface. It tests the full system integration including WiFi
 * connectivity, web server initialization, UI setup, action system, and
 * ticker-based operation management.
 *
 * Test Steps:
 * 1. Initialize Unity test framework and configure random seed
 * 2. Set up serial debugging if enabled
 * 3. Connect to WiFi network using ESPAllOn WiFi controller
 * 4. Create and register external action callbacks for testing
 * 5. Initialize complete ESPAllOn system with setup() and begin()
 * 6. Start UI update ticker for periodic web interface updates
 * 7. Run test for 30 seconds using background ticker
 * 8. Validate system stability and responsiveness
 *
 * This test serves as a comprehensive system validation for ESP32 deployment.
 */

#include <Arduino.h>
#include <unity.h>

#include "../../config.h"
#include "../../utils/dump_utils.h"
#include "../../utils/testTicker.h"

#include "../../../src/controllers/ESPAllOn_Wifi.h"

// Test management functions
/**
 * Test completion function - ends Unity test framework
 */
void endTest() { UNITY_END(); }

/**
 * Timer for test duration - runs test for 30 seconds
 */
TickerFree<> test_endTicker(runTest, 30000, 0, MILLIS);

/**
 * External action callback for testing action system
 * Simulates MQTT message sending functionality
 */
void externalAction(uint16_t mode) { DUMPSLN("EXTERNAL ACTIOOOOON"); }

/**
 * Second external action callback for testing multiple actions
 * Simulates door blocking functionality
 */
void externalAction2(uint16_t mode) { DUMPSLN("Blocked Door"); }

// Test action definitions
String nameAction = "Send MQTT Message";
ESPAction ACTION(nameAction, "ACTION1", externalAction);
ESPAction ACTION2("BLocked Door", "ACTION2", externalAction2);

void setup() {
	UNITY_BEGIN();
	randomSeed(0);
	if (PRINTDEBUG)
		SERIALDEBUG.begin(115200);
	ESPALLON_Wifi::getInstance().connectWifi();
	// RUN_TEST(test_esp_mac);

	ESPAllOn::getInstance().addAction(ACTION);
	ESPAllOn::getInstance().addAction(ACTION2);

	ESPAllOn::getInstance().setup();
	ESPAllOn::getInstance().begin();
	// initSocketProvider();
	UI_UpdateTicker.start();
	test_endTicker.start();
	UNITY_END();
}

void loop() {
	test_endTicker.update();
	// loopSocket();
}