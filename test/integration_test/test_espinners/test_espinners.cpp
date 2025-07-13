#include <Arduino.h>
#include <config.h>
#include <utils.h>

#include "../../config.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "../../utils/testTicker.h"

void executeDelayedCommand() {
	// Espera 10 segundos
	std::this_thread::sleep_for(std::chrono::seconds(10));
	// Ejecuta el comando después de la espera
	int returnCode = system("node ../node_test/src_test/puppeteer_test.js");
	std::cout << "Código de retorno: " << returnCode << std::endl;
}

int main() {
	std::cout << "El hilo principal continúa ejecutándose..." << std::endl;
	// Lanza el hilo que maneja la espera y la ejecución del comando
	std::thread worker(executeDelayedCommand);
	// Hace que el hilo principal espere a que el trabajador termine
	worker.join();
	std::cout << "El hilo de ejecución ha completado." << std::endl;
	return 0;
}

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

void addGPIOESPinner() {
	uint16_t gpio_ref =
		searchByLabel(elementToParentMap.begin()->second, "ESPinnerType");
	Control *typeGPIOController = ESPUI.getControl(gpio_ref);
	typeGPIOController->value = GPIO_LABEL;
	saveElement_callback(typeGPIOController, B_UP);
}

void addStepperESPinner() {
	uint16_t stepper_ref =
		searchByLabel(elementToParentMap.begin()->second, "ESPinnerType");
	Control *typeStepperController = ESPUI.getControl(stepper_ref);
	typeStepperController->value = STEPPER_LABEL;
	saveElement_callback(typeStepperController, B_UP);
}

void addRFIDESPinner() {
	uint16_t rfid_ref =
		searchByLabel(elementToParentMap.begin()->second, "ESPinnerType");
	Control *typeRFIDController = ESPUI.getControl(rfid_ref);
	typeRFIDController->value = RFID_LABEL;
	saveElement_callback(typeRFIDController, B_UP);
}

TickerFree<> test_runTicker(runTest, 2000, 0, MILLIS);
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
	testFunctions.push_back(addStepperESPinner);
	testFunctions.push_back(addRFIDESPinner);

	test_runTicker.start();
}

void loop() { test_runTicker.update(); }