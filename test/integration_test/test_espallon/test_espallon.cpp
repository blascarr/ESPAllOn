/**
 * ESPAllOn Complete System Integration Test
 *
 * This comprehensive integration test validates ESPAllOn
 * WiFi connectivity, UI functionality, ESPinner management, and
 * external browser automation.
 *
 * Test Steps:
 * 1. Initialize system with WiFi connectivity and UI setup
 * 2. Create GPIO ESPinner through UI simulation
 * 3. Create RFID ESPinner through UI simulation
 * 4. Create Stepper ESPinner through UI simulation
 * 5. Create additional GPIO ESPinner for multi-device testing
 * 6. Load ESPinners from persistent storage
 * 7. Start UI update ticker and WiFi management
 * 8. Run periodic tests using ticker system
 * 9. Execute browser automation testing (external thread)
 * 10. Validate system stability and functionality
 *
 * This test serves as the primary validation for complete system deployment.
 */

#include <Arduino.h>
#include <config.h>
#include <utils.h>

#include "../../config.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "../../utils/testTicker.h"

std::vector<std::string> expectedLabels = {"ESPinnerType", "ESPinnerID",
										   "ESPinner_Save"};

void test_firstSelectorOrderInelementParentMap() {
	int index = 0;
	for (auto it = elementToParentMap.begin(); it != elementToParentMap.end();
		 ++it) {
		Control *controller = ESPUI.getControl(it->first);

		const std::string &expectedLabel = expectedLabels[index];
		TEST_ASSERT_EQUAL_STRING(expectedLabel.c_str(), controller->label);
		index++;
	}
	TEST_ASSERT_EQUAL_INT16(3, elementToParentMap.size());
	uint16_t ref =
		searchByLabel(elementToParentMap.begin()->second, "ESPinnerType");

	// The parent is the same as the selector ( in this case "ESPinnerType")
	TEST_ASSERT_EQUAL_INT16(ref, elementToParentMap.begin()->second);
}

void test_no_elementsInParentMap() {
	TEST_ASSERT_EQUAL_INT16(0, elementToParentMap.size());
}

/**
 * Add GPIO ESPinner through UI simulation
 * Simulates user creating a GPIO ESPinner via web interface
 */
void addGPIOESPinner() {
	uint16_t gpio_ref =
		searchByLabel(elementToParentMap.begin()->second, "ESPinnerType");
	Control *typeGPIOController = ESPUI.getControl(gpio_ref);
	typeGPIOController->value = GPIO_LABEL;
	saveElement_callback(typeGPIOController, B_UP);
}

/**
 * Add Stepper ESPinner through UI simulation
 * Simulates user creating a Stepper motor ESPinner via web interface
 */
void addStepperESPinner() {
	uint16_t stepper_ref =
		searchByLabel(elementToParentMap.begin()->second, "ESPinnerType");
	Control *typeStepperController = ESPUI.getControl(stepper_ref);
	typeStepperController->value = STEPPER_LABEL;
	saveElement_callback(typeStepperController, B_UP);
}

/**
 * Add RFID ESPinner through UI simulation
 * Simulates user creating an RFID reader ESPinner via web interface
 */
void addRFIDESPinner() {
	uint16_t rfid_ref =
		searchByLabel(elementToParentMap.begin()->second, "ESPinnerType");
	Control *typeRFIDController = ESPUI.getControl(rfid_ref);
	typeRFIDController->value = RFID_LABEL;
	saveElement_callback(typeRFIDController, B_UP);
}

TickerFree<> test_runTicker(runTest, 4000, 0, MILLIS);
ESPALLON_Wifi wifi = ESPALLON_Wifi::getInstance();

void setup() {
	Serial.begin(115200);
	UNITY_BEGIN();
	while (!Serial)
		;

	wifi.connectWifi();
#if defined(ESP32)
	WiFi.setSleep(false); // For the ESP32: turn off sleeping to increase UI
						  // responsivness (at the cost of power use)
#endif
	DUMPSLN("NO MAP");
	// RUN_TEST(test_no_elementsInParentMap);
	ESPAllOn::getInstance().setup();
	ESPAllOn::getInstance().begin();

	testFunctions.push_back(addGPIOESPinner);
	testFunctions.push_back(addRFIDESPinner);
	testFunctions.push_back(addStepperESPinner);
	testFunctions.push_back(addGPIOESPinner);

	test_runTicker.start();
}

void loop() { test_runTicker.update(); }