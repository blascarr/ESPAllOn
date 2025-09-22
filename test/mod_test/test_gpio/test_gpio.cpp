
/**
 * GPIO Module Test
 *
 * This comprehensive test validates the ESPinner_GPIO module functionality.
 *
 * Test Steps:
 * 1. Initialize system and validate empty state
 * 2. Create first GPIO ESPinner (INPUT mode) and validate UI elements
 * 3. Test invalid pin input handling and validation
 * 4. Save valid GPIO configuration and verify pin manager attachment
 * 5. Modify GPIO pin assignment and test pin detachment/attachment
 * 6. Create second GPIO ESPinner (OUTPUT mode) with different pin
 * 7. Validate multiple ESPinners coexistence and pin exclusivity
 * 8. Remove first ESPinner and verify cleanup operations
 * 9. Validate storage persistence throughout all operations
 */

#include <Arduino.h>
#include <config.h>
#include <set>
#include <utils.h>

#include "../../config.h"
#include "../../utils/testTicker.h"

std::vector<std::string> expectedLabels = {ESPINNERTYPE_LABEL, ESPINNERID_LABEL,
										   SAVEESPINNER_LABEL};
uint16_t firstESPinnerSelector;
Control *typeGPIOController;
uint16_t secondESPinnerSelector;
std::set<uint16_t> currentParentRefs;

std::set<uint16_t>
collectParentRefs(const std::map<uint16_t, uint16_t> &elementToParentMap) {
	std::set<uint16_t> allParents;

	for (const auto &pair : elementToParentMap) {
		allParents.insert(pair.second);
	}
	return allParents;
}

std::set<uint16_t> filterParentRef(std::set<uint16_t> &setListToFilter,
								   uint16_t firstESPinnerSelector) {
	std::set<uint16_t> setListFiltered = setListToFilter;
	setListFiltered = std::move(setListToFilter);
	setListFiltered.erase(firstESPinnerSelector);
	return setListFiltered;
}

void test_firstSelectorOrderInelementParentMap() {
	int index = 0;
	for (auto it = elementToParentMap.begin(); it != elementToParentMap.end();
		 ++it) {
		Control *controller = ESPUI.getControl(it->first);

		const std::string &expectedLabel = expectedLabels[index];
		TEST_ASSERT_EQUAL_STRING(expectedLabel.c_str(), controller->label);
		index++;
	}

	TEST_ASSERT_EQUAL_INT16(1, controlReferences.size());
	TEST_ASSERT_EQUAL_INT16(3, elementToParentMap.size());
	uint16_t ref =
		searchByLabel(elementToParentMap.begin()->second, ESPINNERTYPE_LABEL);

	// The parent is the same as the selector ( in this case "ESPinnerType")
	TEST_ASSERT_EQUAL_INT16(elementToParentMap.begin()->second, ref);

#if defined(ESP32)
	TEST_ASSERT_EQUAL_INT16(
		7, ESPAllOnPinManager::getInstance().getPINMap().size());
#endif

#if defined(ESP8266)
	TEST_ASSERT_EQUAL_INT16(
		7, ESPAllOnPinManager::getInstance().getPINMap().size());
#endif
}

void test_no_elementsInParentMap() {
	TEST_ASSERT_EQUAL_INT16(0, controlReferences.size());
	TEST_ASSERT_EQUAL_INT16(0, elementToParentMap.size());
}

void assertControlValue(uint16_t input_ref, const String &label,
						const String &expectedValue) {
	uint16_t ref = searchByLabel(input_ref, label.c_str());
	if (ref != 0) {
		const char *actualValue = ESPUI.getControl(ref)->value.c_str();
		TEST_ASSERT_EQUAL_STRING(expectedValue.c_str(), actualValue);
	} else {
		TEST_FAIL_MESSAGE("Control reference not found");
	}
}

// -------------------------------- //
// ------- Assertion Model -------- //
// -------------------------------- //

struct ControlAssertion {
	uint16_t selector;
	String label;
	String expectedValue;
};

void assertControlValues(const std::vector<ControlAssertion> &assertions) {
	for (const auto &assertion : assertions) {
		assertControlValue(assertion.selector, assertion.label,
						   assertion.expectedValue);
	}
}

void removedControl(uint16_t input_ref, const String &label) {
	uint16_t ref = searchByLabel(input_ref, label.c_str());
	TEST_ASSERT_EQUAL_INT16(0, ref);
}

Persistance ESPinnerManager(nullptr, &storage);

bool compareESPinnerGPIO(const ESPinner_GPIO &expected_ESPinner_GPIO,
						 const ESPinner_GPIO &actual_ESPinner_GPIO) {
	return expected_ESPinner_GPIO.gpio == actual_ESPinner_GPIO.gpio &&
		   expected_ESPinner_GPIO.GPIO_mode == actual_ESPinner_GPIO.GPIO_mode &&
		   expected_ESPinner_GPIO.mod == actual_ESPinner_GPIO.mod &&
		   expected_ESPinner_GPIO.ID == actual_ESPinner_GPIO.ID;
}

void loadStorage(const ESPinner_GPIO expectedList[]) {
	DynamicJsonDocument doc(256);
	String serialized = ESPinnerManager.loadData(ESPinner_Path);
	DUMP("Dataloaded: ", serialized);
	DeserializationError error = deserializeJson(doc, serialized);
	uint8_t espinner_index = 0;
	if (!error) {
		if (!doc.is<JsonArray>()) {
			DUMPSLN("ERROR: NO JSON ARRAY FORMAT.");
			return;
		}
		JsonArray array = doc.as<JsonArray>();
		for (JsonDocument obj : array) {
			ESPinner_GPIO espinner;
			String output;
			serializeJson(obj, output);
			espinner.deserializeJSON(output);

			TEST_ASSERT_TRUE(
				compareESPinnerGPIO(expectedList[espinner_index], espinner));
			espinner_index++;
		}
	}
}

void checkESPinnerState(const String &id, int expectedGpio,
						ESPinner_Mod expectedType, GPIOMode expectedMode) {
	ESPinner *espinnerInList =
		ESPinner_Manager::getInstance().findESPinnerById(id);
	if (espinnerInList) {
		ESPinner_GPIO *gpioEspinner =
			static_cast<ESPinner_GPIO *>(espinnerInList);
		if (gpioEspinner) {
			TEST_ASSERT_EQUAL_INT8(expectedGpio, gpioEspinner->gpio);
			TEST_ASSERT_EQUAL_STRING(id.c_str(), gpioEspinner->getID().c_str());
			TEST_ASSERT_EQUAL(expectedType, gpioEspinner->getType());
			TEST_ASSERT_EQUAL(expectedMode, gpioEspinner->getGPIOMode());
		} else {
			Serial.println("Error: ESPinner_GPIO Type Not Found");
		}
	} else {
		Serial.println("ESPinner ID Not Found In List");
	}
}

void test_void_GPIOespinner() {
	std::vector<ControlAssertion> void_GPIO_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "GPIO"},
		{firstESPinnerSelector, GPIO_MODESELECTOR_LABEL, "INPUT"},
		{firstESPinnerSelector, GPIO_PINSELECTOR_LABEL, "0"},
		{firstESPinnerSelector, GPIO_SAVE_LABEL, GPIO_SAVE_VALUE},
		{firstESPinnerSelector, REMOVEESPINNER_LABEL, REMOVEESPINNER_VALUE}};

	assertControlValues(void_GPIO_assertions);

	// ESPAllOn_Manager Espinners Void
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(0, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());
}

void test_wrong_GPIOespinner() {
	std::vector<ControlAssertion> wrong_GPIO_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "GPIO"},
		{firstESPinnerSelector, GPIO_MODESELECTOR_LABEL, "INPUT"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_INPUT"},
		// {firstESPinnerSelector, GPIO_SELECT_LABEL, "Select"},
		{firstESPinnerSelector, GPIO_PINSELECTOR_LABEL, "asdfg"},
		{firstESPinnerSelector, GPIO_SAVE_LABEL, GPIO_SAVE_VALUE},
		{firstESPinnerSelector, REMOVEESPINNER_LABEL, REMOVEESPINNER_VALUE}};

	assertControlValues(wrong_GPIO_assertions);

	// ESPAllOn_Manager Espinners Void
	// Check EspAllOn_Pin_manager Status Not Connected
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(0, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());
}

void test_saved_GPIOespinner() {
	std::vector<ControlAssertion> saved_GPIO_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "GPIO"},
		{firstESPinnerSelector, GPIO_MODESELECTOR_LABEL, "INPUT"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_INPUT"},
		{firstESPinnerSelector, GPIO_PINSELECTOR_LABEL, "12"},
		{firstESPinnerSelector, GPIO_SAVE_LABEL, GPIO_SAVE_VALUE},
		{firstESPinnerSelector, REMOVEESPINNER_LABEL, REMOVEESPINNER_VALUE}};

	assertControlValues(saved_GPIO_assertions);

	// Should return MEMORY FAIL: LABEL NOT FOUND because it has been removed
	removedControl(firstESPinnerSelector, GPIO_SELECT_LABEL);

	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());

	ESPinner_GPIO expected_espinner = ESPinner_GPIO(GPIOMode::Input);
	expected_espinner.setGPIO(12);
	expected_espinner.setID("TEST_INPUT");
	ESPinner_GPIO expectedList[] = {expected_espinner};

	checkESPinnerState(expected_espinner.getID(), expected_espinner.getGPIO(),
					   expected_espinner.getType(),
					   expected_espinner.getGPIOMode());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //

	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeGPIOController->value.toInt()));

#if defined(ESP32)
	TEST_ASSERT_EQUAL_INT16(
		8, ESPAllOnPinManager::getInstance().getPINMap().size());
#endif

#if defined(ESP8266)
	TEST_ASSERT_EQUAL_INT16(
		8, ESPAllOnPinManager::getInstance().getPINMap().size());
#endif

	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //
	loadStorage(expectedList);
}

void test_modified_GPIOespinner() {
	std::vector<ControlAssertion> modified_GPIO_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "GPIO"},
		{firstESPinnerSelector, GPIO_MODESELECTOR_LABEL, "INPUT"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_INPUT"},
		{firstESPinnerSelector, GPIO_PINSELECTOR_LABEL, "13"},
		{firstESPinnerSelector, GPIO_SAVE_LABEL, GPIO_SAVE_VALUE},
		{firstESPinnerSelector, REMOVEESPINNER_LABEL, REMOVEESPINNER_VALUE}};

	assertControlValues(modified_GPIO_assertions);

	// Should return MEMORY FAIL: LABEL NOT FOUND because it has been removed
	removedControl(firstESPinnerSelector, GPIO_SELECT_LABEL);

	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());

	ESPinner_GPIO expected_espinner = ESPinner_GPIO(GPIOMode::Input);
	expected_espinner.setGPIO(13);
	expected_espinner.setID("TEST_INPUT");
	ESPinner_GPIO expectedList[] = {expected_espinner};

	checkESPinnerState(expected_espinner.getID(), expected_espinner.getGPIO(),
					   expected_espinner.getType(),
					   expected_espinner.getGPIOMode());

	DUMPLN("ESPINNER PIN ", typeGPIOController->value.toInt());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeGPIOController->value.toInt()));

#if defined(ESP32)
	TEST_ASSERT_EQUAL_INT16(
		8, ESPAllOnPinManager::getInstance().getPINMap().size());
#endif

#if defined(ESP8266)
	TEST_ASSERT_EQUAL_INT16(
		8, ESPAllOnPinManager::getInstance().getPINMap().size());
#endif

	// Previous PIN is detached

	// ----- Controllers Review ------ //
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(12));
	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	uint16_t parentRef = getParentId(elementToParentMap, firstESPinnerSelector);
	uint16_t controllerId = searchByLabel(parentRef, GPIO_SWITCH_LABEL);
	uint16_t controllerRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, GPIO_SWITCH_LABEL);
	TEST_ASSERT_EQUAL(
		parentRef,
		ESPinner_Manager::getInstance().findRefByControllerId(controllerRef));
	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //

	loadStorage(expectedList);
}

void test_second_void_GPIOespinner() {
	std::vector<ControlAssertion> void_GPIO_assertions = {
		{secondESPinnerSelector, ESPINNERTYPE_LABEL, "GPIO"},
		{secondESPinnerSelector, GPIO_MODESELECTOR_LABEL, "INPUT"},
		{secondESPinnerSelector, GPIO_PINSELECTOR_LABEL, "0"},
		{secondESPinnerSelector, GPIO_SELECT_LABEL, GPIO_SELECT_VALUE},
		{secondESPinnerSelector, GPIO_SAVE_LABEL, GPIO_SAVE_VALUE},
		{secondESPinnerSelector, REMOVEESPINNER_LABEL, REMOVEESPINNER_VALUE}};

	assertControlValues(void_GPIO_assertions);

	// ESPAllOn_Manager Espinners Void
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(3, controlReferences.size());

	// ----- Controllers Review ------ //
	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());
}

void test_second_GPIOespinner() {
	std::vector<ControlAssertion> void_GPIO_assertions = {
		{secondESPinnerSelector, ESPINNERTYPE_LABEL, "GPIO"},
		{secondESPinnerSelector, GPIO_MODESELECTOR_LABEL, "OUTPUT"},
		{secondESPinnerSelector, GPIO_PINSELECTOR_LABEL, "5"},
		{secondESPinnerSelector, GPIO_SAVE_LABEL, GPIO_SAVE_VALUE},
		{secondESPinnerSelector, REMOVEESPINNER_LABEL, REMOVEESPINNER_VALUE}};

	assertControlValues(void_GPIO_assertions);

	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(2, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(3, controlReferences.size());

	ESPinner_GPIO first_espinner = ESPinner_GPIO(GPIOMode::Input);
	first_espinner.setGPIO(13);
	first_espinner.setID("TEST_INPUT");

	ESPinner_GPIO expected_espinner = ESPinner_GPIO(GPIOMode::Output);
	expected_espinner.setGPIO(5);
	expected_espinner.setID("TEST_OUTPUT");
	ESPinner_GPIO expectedList[] = {first_espinner, expected_espinner};

	checkESPinnerState(expected_espinner.getID(), expected_espinner.getGPIO(),
					   expected_espinner.getType(),
					   expected_espinner.getGPIOMode());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //

	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeGPIOController->value.toInt()));
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(13));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(12));

#if defined(ESP32)
	TEST_ASSERT_EQUAL_INT16(
		9, ESPAllOnPinManager::getInstance().getPINMap().size());
#endif

#if defined(ESP8266)
	TEST_ASSERT_EQUAL_INT16(
		9, ESPAllOnPinManager::getInstance().getPINMap().size());
#endif

	// ----- Controllers Review ------ //
	ESPinner_Manager::getInstance().debugController();
	ESPinner_Manager::getInstance().debugUIRelation();
	TEST_ASSERT_EQUAL(
		2, ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		2, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //
	loadStorage(expectedList);
}

void test_firstESPinner_removed() {
	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();

	// TODO : Review why is not removing Last ESPinner
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());

	checkESPinnerState("TEST_OUTPUT", 5, ESPinner_Mod::GPIO, GPIOMode::Output);

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeGPIOController->value.toInt()));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(13));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(12));

#if defined(ESP32)
	TEST_ASSERT_EQUAL_INT16(
		8, ESPAllOnPinManager::getInstance().getPINMap().size());
#endif

#if defined(ESP8266)
	TEST_ASSERT_EQUAL_INT16(
		8, ESPAllOnPinManager::getInstance().getPINMap().size());
#endif
	// ----- Controllers Review ------ //
	ESPinner_Manager::getInstance().debugController();
	ESPinner_Manager::getInstance().debugUIRelation();

	DUMPLN("CONTROLLER SIZE: ",
		   ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getControllerMap().size());

	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());
}

void setup() {
	Serial.begin(115200);
	UNITY_BEGIN();
	// debugMap(elementToParentMap);
	RUN_TEST(test_no_elementsInParentMap);
	ESPAllOn::getInstance().setup(); // Executes ESPinnerSelector()

	// debugMap(elementToParentMap);

	RUN_TEST(test_firstSelectorOrderInelementParentMap);

	// ---------------------------------------------//
	// Define GPIO as INPUT and ID = TEST_INPUT
	// Search first ESPinnerSelector created
	// ---------------------------------------------//

	firstESPinnerSelector = elementToParentMap.begin()->second;
	uint16_t espinnerType_ref =
		searchByLabel(firstESPinnerSelector, ESPINNERTYPE_LABEL);
	Control *typeController = ESPUI.getControl(espinnerType_ref);
	typeController->value = GPIO_LABEL; // This is important to be GPIO

	uint16_t gpioID_ref =
		searchByLabel(firstESPinnerSelector, ESPINNERID_LABEL);
	Control *typeGPIOIDController = ESPUI.getControl(gpioID_ref);
	typeGPIOIDController->value = "TEST_INPUT";

	// Click on Save Generic ESPinner and creates a new one
	saveElement_callback(typeController, B_UP);
	debugMap(elementToParentMap);

	RUN_TEST(test_void_GPIOespinner);

	// ---------------------------------------------//
	// GPIO_MODESELECTOR_LABEL
	// REVIEW GPIO STRUCTURE
	// ---------------------------------------------//
	uint16_t gpioMode_ref =
		searchByLabel(firstESPinnerSelector, GPIO_MODESELECTOR_LABEL);
	Control *typeGPIOMODEController = ESPUI.getControl(gpioMode_ref);
	typeGPIOMODEController->value = "INPUT";

	// Input Wrong Number
	uint16_t gpio_ref =
		searchByLabel(firstESPinnerSelector, GPIO_PINSELECTOR_LABEL);
	typeGPIOController = ESPUI.getControl(gpio_ref);
	typeGPIOController->value = "asdfg";

	// Click On save, but not saved because of wrong number in GPIO PIN
	GPIOSelector_callback(typeGPIOController, B_UP);
	saveButtonGPIOCheck(firstESPinnerSelector, GPIO_PINSELECTOR_LABEL,
						gpio_action, GPIO_SAVE_LABEL);
	RUN_TEST(test_wrong_GPIOespinner);

	// ---------------------------------------------//
	// Select a correct Number
	// Click On Save GPIO
	// ---------------------------------------------//
	typeGPIOController->value = "12";
	GPIOSelector_callback(typeGPIOController, B_UP);
	saveButtonGPIOCheck(firstESPinnerSelector, GPIO_PINSELECTOR_LABEL,
						gpio_action, GPIO_SAVE_LABEL);
	debugMap(elementToParentMap);
	RUN_TEST(test_saved_GPIOespinner);

	// ---------------------------------------------//
	// CHANGE PIN NUMBER TO OTHER DIFFERENT
	// Check Attached and detached PINs
	// ---------------------------------------------//
	typeGPIOController->value = "13";
	saveGPIO_callback(typeGPIOController, B_UP);
	debugMap(elementToParentMap);
	RUN_TEST(test_modified_GPIOespinner);

	// -------------------------------------------- //
	// ----------- Second GUI ESPinner ------------ //
	// -------------------------------------------- //

	std::set<uint16_t> ListRefs = collectParentRefs(elementToParentMap);
	currentParentRefs = filterParentRef(ListRefs, firstESPinnerSelector);

	secondESPinnerSelector = *currentParentRefs.begin();

	// Create a new GPIO
	uint16_t second_espinnerType_ref =
		searchByLabel(secondESPinnerSelector, ESPINNERTYPE_LABEL);
	Control *second_typeController = ESPUI.getControl(second_espinnerType_ref);
	second_typeController->value = GPIO_LABEL; // This is important to be GPIO

	uint16_t second_gpioID_ref =
		searchByLabel(secondESPinnerSelector, ESPINNERID_LABEL);
	Control *second_typeGPIOIDController = ESPUI.getControl(second_gpioID_ref);
	second_typeGPIOIDController->value = "TEST_OUTPUT";

	// Click on Save Generic ESPinner and creates a new one
	saveElement_callback(second_typeController, B_UP);
	debugMap(elementToParentMap);
	RUN_TEST(test_second_void_GPIOespinner);

	uint16_t second_gpioMode_ref =
		searchByLabel(secondESPinnerSelector, GPIO_MODESELECTOR_LABEL);
	Control *second_typeGPIOMODEController =
		ESPUI.getControl(second_gpioMode_ref);
	second_typeGPIOMODEController->value = "OUTPUT";
	uint16_t second_gpio_ref =
		searchByLabel(secondESPinnerSelector, GPIO_PINSELECTOR_LABEL);
	typeGPIOController = ESPUI.getControl(second_gpio_ref);
	typeGPIOController->value = "5";

	// Click On save for GPIO PIN
	saveButtonGPIOCheck(secondESPinnerSelector, GPIO_PINSELECTOR_LABEL,
						gpio_action, GPIO_SAVE_LABEL);
	saveGPIO_callback(second_typeController, B_UP);
	// saveElement_callback(second_typeController, B_UP);
	// RUN_TEST([&]() { test_controlReferenceExists(controlReferences, testRef);
	// });
	debugMap(elementToParentMap);
	RUN_TEST(test_second_GPIOespinner);

	// Define GPIO as OUTPUT and ID = TEST_OUTPUT
	// Input the last GPIO Number which is occupied
	// This should not be referenced again

	// Change input GPIO
	// Click On Save GPIO

	// -------------------------------------------- //
	// ----------- Remove First GPIO ------------ //
	// -------------------------------------------- //

	uint16_t espinnerRemove_ref =
		searchByLabel(firstESPinnerSelector, REMOVEESPINNER_LABEL);
	Control *second_removeController = ESPUI.getControl(espinnerRemove_ref);
	removeGPIO_callback(second_removeController, B_UP);

	RUN_TEST(test_firstESPinner_removed);

	UNITY_END();
}

void loop() {}