#include "../../utils/testTicker.h"

#include "../../../src/controllers/ESPAllOn_Wifi.h"

void runWifiTest() {
	if (currentTestIndex < testFunctions.size()) {
		testFunctions[currentTestIndex](); // Execute Current Test
		currentTestIndex++;				   // Next Index
	} else {
		DUMPSLN("All tests completed");
		ESPALLON_Wifi::getInstance();
		ESPALLON_Wifi::getInstance().stop();
		UNITY_END();
	}
}

TickerFree<> test_runTicker(runWifiTest, 2000, 0, MILLIS);

void test_wifi() {
	bool WifiConnected = ESPALLON_Wifi::getInstance().checkWifiConnection();
	TEST_ASSERT_TRUE(WifiConnected);
}

void test_nowifi() {
	bool WifiConnected = ESPALLON_Wifi::getInstance().checkWifiConnection();
	TEST_ASSERT_FALSE(WifiConnected);
}

void isWifiNotConnected() { RUN_TEST(test_nowifi); }
void isWifiConnected() { RUN_TEST(test_wifi); }

void setup() {
	UNITY_BEGIN();
	ESPALLON_Wifi::getInstance().begin();
	RUN_TEST(test_nowifi);
	ESPALLON_Wifi::getInstance().connectTick();
	testFunctions.push_back(isWifiConnected);
	ESPALLON_Wifi::getInstance().start();
	test_runTicker.start();
}

void loop() {
	ESPALLON_Wifi::getInstance().update();
	test_runTicker.update();
}