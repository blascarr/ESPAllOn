#include "../../utils/testTicker.h"

#include "../../../src/controllers/ESPinner.h"

TickerFree<> test_runTicker(runTest, 2000, 0, MILLIS);

void isWifiNotConnected() {}
void isWifiConnected() {}

void setup() {
	UNITY_BEGIN();

	test_runTicker.start();
	UNITY_END();
}

void loop() { test_runTicker.update(); }