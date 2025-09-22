/**
 * WiFi Unit Test
 *
 * This test validates the WiFi connection functionality of the ESPALLON_Wifi
 * controller.
 *
 * Test Steps:
 * 1. Initialize Unity test framework
 * 2. Begin WiFi controller and test initial disconnected state
 * 3. Start WiFi connection process with connectTick()
 * 4. Use ticker to periodically check WiFi connection status
 * 5. Validate successful WiFi connection after delay
 * 6. Stop WiFi controller and end test execution
 */

#include "../../utils/testTicker.h"

#include "../../../src/controllers/ESPAllOn_Wifi.h"

/**
 * Main test runner function executed by ticker
 * Manages sequential execution of WiFi test functions
 */
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

/**
 * Test WiFi connection establishment
 * Validates that WiFi connection is successfully established
 */
void test_wifi() {
	bool WifiConnected = ESPALLON_Wifi::getInstance().checkWifiConnection();
	TEST_ASSERT_TRUE(WifiConnected);
}

/**
 * Test WiFi disconnected state
 * Validates that WiFi is initially disconnected
 */
void test_nowifi() {
	bool WifiConnected = ESPALLON_Wifi::getInstance().checkWifiConnection();
	TEST_ASSERT_FALSE(WifiConnected);
}

/**
 * Wrapper function to test WiFi disconnected state
 */
void isWifiNotConnected() { RUN_TEST(test_nowifi); }

/**
 * Wrapper function to test WiFi connected state
 */
void isWifiConnected() { RUN_TEST(test_wifi); }

void setup() {
	UNITY_BEGIN();
	RUN_TEST(test_nowifi);
	ESPALLON_Wifi::getInstance().begin();
	ESPALLON_Wifi::getInstance().connectTick();
	testFunctions.push_back(isWifiConnected);
	ESPALLON_Wifi::getInstance().start();
	test_runTicker.start();
}

void loop() {
	ESPALLON_Wifi::getInstance().update();
	test_runTicker.update();
}