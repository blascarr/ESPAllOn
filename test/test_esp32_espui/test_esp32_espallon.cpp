#include <Arduino.h>
#include <unity.h>

#include "../config.h"
#include "../socket_utils.h"

// UI handles
void endTest() { UNITY_END(); }
Ticker test_endTicker(endTest, 30000, 0, MILLIS);

void externalAction(uint16_t mode) { Serial.println("EXTERNAL ACTIOOOOON"); }
void externalAction2(uint16_t mode) { Serial.println("Blocked Door"); }

String nameAction = "Send MQTT Message";
ESPAction ACTION(nameAction, "ACTION1", externalAction);
ESPAction ACTION2("BLocked Door", "ACTION2", externalAction2);

void setup() {
	UNITY_BEGIN();
	randomSeed(0);
	if (PRINTDEBUG)
		SERIALDEBUG.begin(115200);
	connectWifi();
	// RUN_TEST(test_esp_mac);

	ESPAllOn::getInstance().addAction(ACTION);
	ESPAllOn::getInstance().addAction(ACTION2);

	ESPAllOn::getInstance().setup();
	ESPAllOn::getInstance().begin();
	initSocketProvider();
	UI_UpdateTicker.start();
	test_endTicker.start();
}

void loop() {
	test_endTicker.update();
	loopSocket();
}