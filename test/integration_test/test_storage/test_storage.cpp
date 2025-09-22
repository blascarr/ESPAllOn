/**
 * Storage Integration Test
 *
 * This integration test validates the persistent storage functionality in a
 * real system environment.
 *
 * Test Steps:
 * 1. Initialize complete ESPAllOn system with WiFi and UI
 * 2. Create multiple ESPinner objects programmatically
 * 3. Push ESPinners to manager and validate storage
 * 4. Test UI-based ESPinner creation through ticker system
 * 5. Validate storage persistence across system operations
 * 6. Test storage retrieval and ESPinner reconstruction
 * 7. Validate system stability with multiple ESPinners
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

TickerFree<> test_runTicker(runTest, 2000, 0, MILLIS);
ESPALLON_Wifi wifi = ESPALLON_Wifi::getInstance();

/**
 * Add GPIO ESPinner through UI for storage testing
 */
void addGPIOESPinner() {
	uint16_t gpio_ref =
		searchByLabel(elementToParentMap.begin()->second, "ESPinnerType");
	Control *typeGPIOController = ESPUI.getControl(gpio_ref);
	typeGPIOController->value = GPIO_LABEL;
	saveElement_callback(typeGPIOController, B_UP);
}

void setup() {
	Serial.begin(115200);
	UNITY_BEGIN();

	wifi.connectWifi();
#if defined(ESP32)
	WiFi.setSleep(false); // For the ESP32: turn off sleeping to increase UI
						  // responsivness (at the cost of power use)
#endif
	// RUN_TEST(test_no_elementsInParentMap);
	ESPAllOn::getInstance().setup();
	ESPAllOn::getInstance().begin();

	auto espinnerGPIO_1 = std::make_unique<ESPinner_GPIO>();
	espinnerGPIO_1->setGPIO(5);
	espinnerGPIO_1->setGPIOMode(GPIOMode::Input);
	espinnerGPIO_1->setID("ESPINNER GPIO 1");

	auto espinnerGPIO_2 = std::make_unique<ESPinner_GPIO>();
	espinnerGPIO_2->setGPIO(10);
	espinnerGPIO_2->setGPIOMode(GPIOMode::Output);
	espinnerGPIO_2->setID("ESPINNER GPIO 2");

	auto espinnerDC = std::make_unique<ESPinner_DC>();
	espinnerDC->setGPIOA(11);
	espinnerDC->setID("ESPINNER DC");

	// Push Espinners
	ESPinner_Manager::getInstance().push(std::move(espinnerGPIO_1));
	ESPinner_Manager::getInstance().push(std::move(espinnerGPIO_2));
	ESPinner_Manager::getInstance().push(std::move(espinnerDC));

	// Test saved in Storage

	// Test add a Espinner from UI
	// Check ESPinnerList and Status

	// Test remove a different ESPinner
	// Check EspinnerList and Pin Status
	testFunctions.push_back(addGPIOESPinner);
	test_runTicker.start();
}

void loop() { test_runTicker.update(); }