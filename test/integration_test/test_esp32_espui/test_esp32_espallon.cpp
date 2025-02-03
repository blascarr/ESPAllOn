#include <Arduino.h>
#include <unity.h>

#include "../../../src/controllers/ESPAllOn_Wifi.h"
#include "../../config.h"
#include "../../utils/socket_utils.h"
#include "../../utils/testTicker.h"

// UI handles
void endTest() { UNITY_END(); }
TickerFree<> test_endTicker(runTest, 30000, 0, MILLIS);

void externalAction(uint16_t mode) { DUMPSLN("EXTERNAL ACTIOOOOON"); }
void externalAction2(uint16_t mode) { DUMPSLN("Blocked Door"); }

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