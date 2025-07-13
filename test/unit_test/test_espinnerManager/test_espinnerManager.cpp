#include "../../config.h"

#include "../../../src/manager/ESPinner_Manager.h"
#include "../../utils/persistance_utils.h"
#include "../../utils/testTicker.h"

TickerFree<> test_runTicker(runTest, 2000, 0, MILLIS);

void isWifiNotConnected() {}
void isWifiConnected() {}

ESPinner_GPIO gpio1;
ESPinner_GPIO gpio2;

void test_is_there_any_file() {
	bool allFilesDeleted = isThereAnyFileInLittleFS();
	TEST_ASSERT_TRUE(allFilesDeleted);
}

void setup() {
	Serial.begin(115200);
	UNITY_BEGIN();
	gpio1.setGPIO(5);

	gpio1.setGPIOMode(GPIOMode::Input);
	ESPinner_Manager::getInstance();
	test_runTicker.start();
	RUN_TEST(test_is_there_any_file);
	UNITY_END();
}

void loop() { test_runTicker.update(); }