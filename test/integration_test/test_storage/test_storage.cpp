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