
#include <Arduino.h>
#include <config.h>
#include <utils.h>

#include "../../config.h"
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

void test_espinners_size() {
	// size_t ESPinnersLength = ESPinner_Manager::getInstance().espinnerSize();
	// size_t es un alias de unsigned int
	// TEST_ASSERT_EQUAL_UINT32(ESPinnersLength, 3);
}

void setup() {
	Serial.begin(115200);
	UNITY_BEGIN();

	DUMPSLN("NO MAP");
	RUN_TEST(test_no_elementsInParentMap);
	ESPAllOn::getInstance().setup();

	RUN_TEST(test_firstSelectorOrderInelementParentMap);
	uint16_t gpio_ref =
		searchByLabel(elementToParentMap.begin()->second, "ESPinnerType");
	Control *typeGPIOController = ESPUI.getControl(gpio_ref);
	typeGPIOController->value = GPIO_LABEL;
	saveElement_callback(typeGPIOController, 1);

	// TODO: ESPinner_MAnager executes UI actions
	// TODO: ESPAllOn_PinManager Configuration

	uint16_t stepper_ref =
		searchByLabel(elementToParentMap.begin()->second, "ESPinnerType");
	Control *typeStepperController = ESPUI.getControl(stepper_ref);
	typeStepperController->value = STEPPER_LABEL;
	saveElement_callback(typeStepperController, 1);

	uint16_t rfid_ref =
		searchByLabel(elementToParentMap.begin()->second, "ESPinnerType");
	Control *typeRFIDController = ESPUI.getControl(rfid_ref);
	typeRFIDController->value = GPIO_LABEL;
	saveElement_callback(typeRFIDController, 1);
	// ESPAllOn::getInstance().begin();
	UNITY_END();
}

void loop() {}