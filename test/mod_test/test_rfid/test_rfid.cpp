
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
		TEST_ASSERT_EQUAL_STRING(controller->label, expectedLabel.c_str());
		index++;
	}
	TEST_ASSERT_EQUAL_INT16(elementToParentMap.size(), 3);
	uint16_t ref =
		searchByLabel(elementToParentMap.begin()->second, "ESPinnerType");

	// The parent is the same as the selector ( in this case "ESPinnerType")
	TEST_ASSERT_EQUAL_INT16(elementToParentMap.begin()->second, ref);
}

void test_no_elementsInParentMap() {
	TEST_ASSERT_EQUAL_INT16(elementToParentMap.size(), 0);
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
	uint16_t rfid_ref =
		searchByLabel(elementToParentMap.begin()->second, "ESPinnerType");
	Control *typeRFIDController = ESPUI.getControl(rfid_ref);
	typeRFIDController->value = RFID_LABEL;
	saveElement_callback(typeRFIDController, B_UP);

	// TODO: ESPinner_MAnager executes UI actions
	// TODO: ESPAllOn_PinManager Configuration

	UNITY_END();
}

void loop() {}