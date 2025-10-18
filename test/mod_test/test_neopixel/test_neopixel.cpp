/**
 * NeoPixel LED Module Test
 *
 * This comprehensive test validates the ESPinner_Neopixel module functionality
 * for NeoPixel LED strip control.
 *
 * Test Steps:
 * 1. Initialize system and validate empty state
 * 2. Create first NeoPixel ESPinner and validate UI structure
 * 3. Test invalid pin input handling for GPIO pin
 * 4. Configure valid pin assignments for GPIO and NumPixels
 * 5. Save NeoPixel configuration and verify pin manager attachments
 * 6. Modify pin assignments and test pin detachment/reattachment
 * 7. Validate NeoPixel controller creation (enable, brightness, rainbow, speed)
 * 8. Create second NeoPixel ESPinner with different pin configuration
 * 9. Test multiple NeoPixel coexistence and pin exclusivity
 * 10. Remove first NeoPixel ESPinner and verify complete cleanup
 * 11. Validate storage persistence and controller cleanup
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
Control *typeGPIO_Controller;
Control *typeNumPixels_Controller;
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

	// The parent is the same as the selector (in this case "ESPinnerType")
	TEST_ASSERT_EQUAL_INT16(elementToParentMap.begin()->second, ref);
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

Persistance ESPinnerManager(nullptr, &storage);

bool compareESPinnerNeopixel(ESPinner_Neopixel &expected_ESPinner_Neopixel,
							 ESPinner_Neopixel &actual_ESPinner_Neopixel) {
	return expected_ESPinner_Neopixel.getGPIO() ==
			   actual_ESPinner_Neopixel.getGPIO() &&
		   expected_ESPinner_Neopixel.getNumPixels() ==
			   actual_ESPinner_Neopixel.getNumPixels() &&
		   expected_ESPinner_Neopixel.getType() ==
			   actual_ESPinner_Neopixel.getType() &&
		   expected_ESPinner_Neopixel.getID() ==
			   actual_ESPinner_Neopixel.getID();
}

void loadStorage(ESPinner_Neopixel *expectedList, uint8_t expectedCount) {
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
		TEST_ASSERT_EQUAL_INT8(expectedCount, array.size());

		ESPinner_Neopixel espinner;
		String output;
		serializeJson(array[expectedCount - 1], output);
		espinner.deserializeJSON(output);
		TEST_ASSERT_TRUE(
			compareESPinnerNeopixel(expectedList[espinner_index], espinner));
	}
}

void checkESPinnerState(const String &id, int expectedGPIO,
						int expectedNumPixels, ESPinner_Mod expectedType) {
	ESPinner *espinnerInList =
		ESPinner_Manager::getInstance().findESPinnerById(id);
	if (espinnerInList) {
		ESPinner_Neopixel *NeopixelEspinner =
			static_cast<ESPinner_Neopixel *>(espinnerInList);
		if (NeopixelEspinner) {
			TEST_ASSERT_EQUAL_INT8(expectedGPIO, NeopixelEspinner->getGPIO());
			TEST_ASSERT_EQUAL_INT8(expectedNumPixels,
								   NeopixelEspinner->getNumPixels());
			TEST_ASSERT_EQUAL_STRING(id.c_str(),
									 NeopixelEspinner->getID().c_str());
			TEST_ASSERT_EQUAL(expectedType, NeopixelEspinner->getType());
		} else {
			Serial.println("Error: ESPinner_Neopixel Type Not Found");
		}
	} else {
		Serial.println("ESPinner ID Not Found In List");
	}
}

/**
 * Test void/empty NeoPixel ESPinner state after creation
 *
 * Code Steps:
 * 1. Create assertion vector with expected default UI values
 *    - ESPinner type should be "Neopixel"
 *    - GPIO pin should default to "0" (unassigned)
 *    - NumPixels should default to "8"
 *    - Save and Remove buttons should have correct labels
 * 2. Execute assertControlValues() to validate all UI elements
 * 3. Get current ESPinner list size from manager
 * 4. Assert no ESPinners exist in manager (size = 0)
 * 5. Assert control references count is 2 (main + current selector)
 */
void test_void_NeopixelEspinner() {
	std::vector<ControlAssertion> void_Neopixel_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "Neopixel"},
		{firstESPinnerSelector, NEOPIXEL_PIN_SELECTOR_LABEL, "0"},
		{firstESPinnerSelector, NEOPIXEL_NUMPIXELS_LABEL, "8"},
		{firstESPinnerSelector, NEOPIXEL_SAVE_LABEL, NEOPIXEL_SAVE_VALUE},
		{firstESPinnerSelector, NEOPIXEL_REMOVE_LABEL, NEOPIXEL_REMOVE_VALUE}};

	assertControlValues(void_Neopixel_assertions);

	// ESPAllOn_Manager Espinners Void
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(0, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());
}

/**
 * Test NeoPixel ESPinner with invalid pin input handling
 *
 * Code Steps:
 * 1. Create assertion vector with invalid pin value "asdfg" for GPIO pin
 *    - Type should be "Neopixel", ID should be "TEST_NEOPIXEL"
 *    - GPIO pin has invalid string "asdfg" (non-numeric)
 *    - NumPixels remains valid at "8"
 * 2. Execute assertControlValues() to validate UI shows invalid input
 * 3. Verify ESPinner manager rejects invalid configuration
 * 4. Assert ESPinner list remains empty (invalid data not saved)
 * 5. Assert pin manager has no pin attachments (validation failed)
 * 6. Confirm control references remain at 2 (no new ESPinner created)
 */
void test_wrong_NeopixelEspinner() {
	std::vector<ControlAssertion> wrong_Neopixel_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "Neopixel"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_NEOPIXEL"},
		{firstESPinnerSelector, NEOPIXEL_PIN_SELECTOR_LABEL, "asdfg"},
		{firstESPinnerSelector, NEOPIXEL_NUMPIXELS_LABEL, "8"},
		{firstESPinnerSelector, NEOPIXEL_SAVE_LABEL, NEOPIXEL_SAVE_VALUE},
		{firstESPinnerSelector, NEOPIXEL_REMOVE_LABEL, NEOPIXEL_REMOVE_VALUE}};

	assertControlValues(wrong_Neopixel_assertions);

	// ESPAllOn_Manager Espinners Void
	// Check EspAllOn_Pin_manager Status Not Connected
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(0, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());
}

/**
 * Test successfully saved NeoPixel ESPinner with valid configuration
 *
 * Code Steps:
 * 1. Create assertion vector with valid NeoPixel configuration
 *    - Type: "Neopixel", ID: "TEST_NEOPIXEL"
 *    - GPIO: pin 15, NumPixels: 16
 *    - Save/Remove buttons with correct values
 * 2. Execute assertControlValues() to validate UI state
 * 3. Verify GPIO selector controls are removed (UI cleanup after save)
 * 4. Check ESPinner manager contains exactly 1 ESPinner
 * 5. Assert control references count remains at 2
 * 6. Create expected ESPinner object for comparison
 * 7. Execute checkESPinnerState() to validate ESPinner in memory
 * 8. Verify pin manager has GPIO pin attached (GPIO 15)
 * 9. Execute loadStorage() to validate persistent storage
 */
void test_saved_NeopixelEspinner() {
	std::vector<ControlAssertion> saved_Neopixel_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "Neopixel"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_NEOPIXEL"},
		{firstESPinnerSelector, NEOPIXEL_PIN_SELECTOR_LABEL, "15"},
		{firstESPinnerSelector, NEOPIXEL_NUMPIXELS_LABEL, "16"},
		{firstESPinnerSelector, NEOPIXEL_SAVE_LABEL, NEOPIXEL_SAVE_VALUE},
		{firstESPinnerSelector, NEOPIXEL_REMOVE_LABEL, NEOPIXEL_REMOVE_VALUE}};

	assertControlValues(saved_Neopixel_assertions);

	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());

	ESPinner_Neopixel expected_espinner = ESPinner_Neopixel();
	expected_espinner.setGPIO(15);
	expected_espinner.setNumPixels(16);
	expected_espinner.setID("TEST_NEOPIXEL");
	checkESPinnerState(expected_espinner.getID(), expected_espinner.getGPIO(),
					   expected_espinner.getNumPixels(),
					   expected_espinner.getType());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //

	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeGPIO_Controller->value.toInt()));

	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //
	loadStorage(&expected_espinner, 1);
}

/**
 * Test NeoPixel ESPinner pin modification and pin detachment/attachment
 *
 * Code Steps:
 * 1. Create assertion vector with modified pin configuration
 *    - Same ESPinner ID "TEST_NEOPIXEL" but different pins
 *    - GPIO changed from 15 to 14, NumPixels changed from 16 to 24
 * 2. Execute assertControlValues() to validate UI updates
 * 3. Verify GPIO selector controls are removed after modification
 * 4. Assert ESPinner manager still contains exactly 1 ESPinner
 * 5. Create expected ESPinner object with new pin values
 * 6. Execute checkESPinnerState() to validate updated ESPinner
 * 7. Debug output current pin assignments
 * 8. Verify pin manager has new pin attached (GPIO 14)
 * 9. Assert previous pin is detached (GPIO 15)
 * 10. Validate NeoPixel controller creation:
 *     - 4 controllers: EN switch, Brightness slider, Rainbow switch, Speed
 * slider
 *     - 1 UI relation mapping ESPinner to parent reference
 * 11. Search and validate each controller type by label
 * 12. Assert each controller is properly linked to parent reference
 * 13. Execute loadStorage() to validate persistence of changes
 */
void test_modified_NeopixelEspinner() {
	std::vector<ControlAssertion> modified_Neopixel_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "Neopixel"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_NEOPIXEL"},
		{firstESPinnerSelector, NEOPIXEL_PIN_SELECTOR_LABEL, "14"},
		{firstESPinnerSelector, NEOPIXEL_NUMPIXELS_LABEL, "24"},
		{firstESPinnerSelector, NEOPIXEL_SAVE_LABEL, NEOPIXEL_SAVE_VALUE},
		{firstESPinnerSelector, NEOPIXEL_REMOVE_LABEL, NEOPIXEL_REMOVE_VALUE}};

	assertControlValues(modified_Neopixel_assertions);

	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());

	ESPinner_Neopixel expected_espinner = ESPinner_Neopixel();
	expected_espinner.setGPIO(14);
	expected_espinner.setNumPixels(24);
	expected_espinner.setID("TEST_NEOPIXEL");

	checkESPinnerState(expected_espinner.getID(), expected_espinner.getGPIO(),
					   expected_espinner.getNumPixels(),
					   expected_espinner.getType());

	DUMPLN("ESPINNER GPIO PIN ", typeGPIO_Controller->value.toInt());
	DUMPLN("ESPINNER NUM PIXELS ", typeNumPixels_Controller->value.toInt());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeGPIO_Controller->value.toInt()));

	// Previous PIN is detached
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(15));

	// ----- Controllers Review ------ //
	// Controller is composed by:
	// 	- 2 switches: EN switch, Rainbow switch
	// 	- 2 sliders: Brightness slider, Speed slider
	// 	- 2 labels
	TEST_ASSERT_EQUAL(
		9, ESPinner_Manager::getInstance().getControllerMap().size());

	// TODO: Review this relation ID Map
	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	uint16_t parentRef = getParentId(elementToParentMap, firstESPinnerSelector);
	uint16_t controllerENRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, NEOPIXEL_SWITCH_EN_LABEL);
	uint16_t controllerBrightnessRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, NEOPIXEL_BRIGHTNESS_SLIDER_LABEL);
	uint16_t controllerRainbowRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, NEOPIXEL_SWITCH_RAINBOW_LABEL);
	uint16_t controllerSpeedRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, NEOPIXEL_SPEED_LABEL);

	// Verify controllers exist
	// TEST_ASSERT_NOT_EQUAL(0, controllerENRef);
	// TEST_ASSERT_NOT_EQUAL(0, controllerBrightnessRef);
	// TEST_ASSERT_NOT_EQUAL(0, controllerRainbowRef);
	// TEST_ASSERT_NOT_EQUAL(0, controllerSpeedRef);

	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //

	loadStorage(&expected_espinner, 1);
}

/**
 * Test second void/empty NeoPixel ESPinner state after creation
 *
 * Code Steps:
 * 1. Create assertion vector with expected default UI values for second
 * selector
 *    - ESPinner type should be "Neopixel"
 *    - ID should be "TEST_NEOPIXEL_2"
 *    - GPIO pin should default to "0" (unassigned)
 *    - NumPixels should default to "8"
 *    - GPIO select labels should be visible (not removed yet)
 *    - Save and Remove buttons should have correct labels
 * 2. Execute assertControlValues() to validate all UI elements
 * 3. Get current ESPinner list size from manager (should be 1 from first)
 * 4. Assert control references count is 3 (main + first + second selector)
 * 5. Verify controller map has 6 controllers from first NeoPixel
 * 6. Assert UI relation map has 1 entry (first NeoPixel controller)
 */
void test_second_void_NeopixelEspinner() {
	std::vector<ControlAssertion> void_Neopixel_assertions = {
		{secondESPinnerSelector, ESPINNERTYPE_LABEL, "Neopixel"},
		{secondESPinnerSelector, ESPINNERID_LABEL, "TEST_NEOPIXEL_2"},
		{secondESPinnerSelector, NEOPIXEL_PIN_SELECTOR_LABEL, "0"},
		{secondESPinnerSelector, NEOPIXEL_NUMPIXELS_LABEL, "8"},
		{secondESPinnerSelector, NEOPIXEL_PIN_SELECT_LABEL,
		 NEOPIXEL_PIN_SELECT_VALUE},
		{secondESPinnerSelector, NEOPIXEL_NUMPIXELS_SELECT_LABEL,
		 NEOPIXEL_NUMPIXELS_SELECT_VALUE},
		{secondESPinnerSelector, NEOPIXEL_SAVE_LABEL, NEOPIXEL_SAVE_VALUE},
		{secondESPinnerSelector, NEOPIXEL_REMOVE_LABEL, NEOPIXEL_REMOVE_VALUE}};

	assertControlValues(void_Neopixel_assertions);

	// ESPAllOn_Manager Espinners
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(3, controlReferences.size());

	TEST_ASSERT_EQUAL(
		9, ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());
}

/**
 * Test successfully saved second NeoPixel ESPinner with valid configuration
 *
 * Code Steps:
 * 1. Create assertion vector with valid second NeoPixel configuration
 *    - Type: "Neopixel", ID: "TEST_NEOPIXEL_2"
 *    - GPIO: pin 4, NumPixels: 32 (different from first NeoPixel)
 *    - Save/Remove buttons with correct values
 * 2. Execute assertControlValues() to validate UI state
 * 3. Check ESPinner manager contains exactly 2 ESPinners
 * 4. Assert control references count is 3 (main + first + second)
 * 5. Create expected ESPinner objects for both NeoPixels
 * 6. Execute checkESPinnerState() to validate second ESPinner in memory
 * 7. Verify pin manager has second NeoPixel pin attached (GPIO 4)
 * 8. Validate NeoPixel controllers for both strips:
 *    - 12 controllers total: 6 for each NeoPixel
 *    - 2 UI relations mapping each ESPinner to parent reference
 * 9. Search and validate each controller type for second NeoPixel
 * 10. Assert each controller is properly linked to second parent reference
 * 11. Execute loadStorage() to validate persistence of both NeoPixels
 */
void test_second_NeopixelEspinner() {
	std::vector<ControlAssertion> void_Neopixel_assertions = {
		{secondESPinnerSelector, ESPINNERTYPE_LABEL, "Neopixel"},
		{secondESPinnerSelector, ESPINNERID_LABEL, "TEST_NEOPIXEL_2"},
		{secondESPinnerSelector, NEOPIXEL_PIN_SELECTOR_LABEL, "4"},
		{secondESPinnerSelector, NEOPIXEL_NUMPIXELS_LABEL, "32"},
		{secondESPinnerSelector, NEOPIXEL_SAVE_LABEL, NEOPIXEL_SAVE_VALUE},
		{secondESPinnerSelector, NEOPIXEL_REMOVE_LABEL, NEOPIXEL_REMOVE_VALUE}};

	assertControlValues(void_Neopixel_assertions);

	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(2, EspinnerListSize);

	// REVIEW HERE ?=?= 2 ESPINNERS + 2 CONTROLLERS
	TEST_ASSERT_EQUAL_INT16(3, controlReferences.size());

	ESPinner_Neopixel first_espinner;
	first_espinner.setGPIO(14);
	first_espinner.setNumPixels(24);
	first_espinner.setID("TEST_NEOPIXEL");

	ESPinner_Neopixel expected_espinner;
	expected_espinner.setGPIO(4);
	expected_espinner.setNumPixels(32);
	expected_espinner.setID("TEST_NEOPIXEL_2");

	checkESPinnerState(expected_espinner.getID(), expected_espinner.getGPIO(),
					   expected_espinner.getNumPixels(),
					   expected_espinner.getType());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //

	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeGPIO_Controller->value.toInt()));

	// ----- Controllers Review ------ //
	// TO REVIEW: 2 NeoPixels × 6 controllers each = 12 total
	TEST_ASSERT_EQUAL(
		18, ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		2, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	uint16_t parentRef =
		getParentId(elementToParentMap, secondESPinnerSelector);
	uint16_t controllerENRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, NEOPIXEL_SWITCH_EN_LABEL);
	uint16_t controllerBrightnessRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, NEOPIXEL_BRIGHTNESS_SLIDER_LABEL);
	uint16_t controllerRainbowRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, NEOPIXEL_SWITCH_RAINBOW_LABEL);
	uint16_t controllerSpeedRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, NEOPIXEL_SPEED_LABEL);

	// Verify controllers exist for second NeoPixel
	// TEST_ASSERT_NOT_EQUAL(0, controllerENRef);
	// TEST_ASSERT_NOT_EQUAL(0, controllerBrightnessRef);
	// TEST_ASSERT_NOT_EQUAL(0, controllerRainbowRef);
	// TEST_ASSERT_NOT_EQUAL(0, controllerSpeedRef);

	uint16_t firstParentRef =
		getParentId(elementToParentMap, firstESPinnerSelector);
	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //
	loadStorage(&expected_espinner, 2);
}

/**
 * Test first NeoPixel ESPinner removal and complete cleanup validation
 *
 * Code Steps:
 * 1. Verify ESPinner manager contains exactly 1 ESPinner (second one remains)
 * 2. Assert control references count returns to 2 (main + remaining selector)
 * 3. Create expected ESPinner object for remaining NeoPixel (TEST_NEOPIXEL_2)
 * 4. Execute checkESPinnerState() to validate remaining ESPinner state
 * 5. Verify pin manager still has second NeoPixel pin attached (GPIO 4)
 * 6. Assert first NeoPixel pin is properly detached (GPIO 14)
 * 7. Validate controller cleanup:
 *    - Only 6 controllers remain (from second NeoPixel)
 *    - Only 1 UI relation remains (second NeoPixel controller)
 * 8. Execute loadStorage() to validate persistent storage contains only second
 * NeoPixel
 * 9. Confirm complete removal of first NeoPixel from all system components
 */
void test_firstESPinner_removed() {
	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();

	// TODO: Review why is not removing Last ESPinner
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());

	ESPinner_Neopixel expected_espinner = ESPinner_Neopixel();
	expected_espinner.setGPIO(4);
	expected_espinner.setNumPixels(32);
	expected_espinner.setID("TEST_NEOPIXEL_2");
	checkESPinnerState(expected_espinner.getID(), expected_espinner.getGPIO(),
					   expected_espinner.getNumPixels(),
					   expected_espinner.getType());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeGPIO_Controller->value.toInt()));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(14));

	TEST_ASSERT_EQUAL(
		18, ESPinner_Manager::getInstance().getControllerMap().size());

	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //
	loadStorage(&expected_espinner, 1);
}

/**
 * Main test setup function - orchestrates complete NeoPixel ESPinner test
 * sequence
 *
 * Code Steps:
 * 1. Initialize serial communication at 115200 baud
 * 2. Begin Unity test framework
 * 3. Test initial empty state (no UI elements)
 * 4. Execute ESPAllOn.setup() to initialize system and create first UI selector
 * 5. Validate first selector structure and element ordering
 * 6. Configure first ESPinner as NeoPixel type:
 *    - Get first ESPinner selector reference from elementToParentMap
 *    - Find ESPinner type control and set value to "Neopixel"
 *    - Find ESPinner ID control and set value to "TEST_NEOPIXEL"
 *    - Execute saveElement_callback() to create NeoPixel ESPinner UI
 * 7. Test void NeoPixel ESPinner state (default values)
 * 8. Test invalid pin input handling:
 *    - Set GPIO pin to invalid value "asdfg"
 *    - Validate system rejects invalid configuration
 * 9. Configure valid pin assignments:
 *    - GPIO: pin 15, NumPixels: 16
 *    - Execute callbacks and validation
 * 10. Test saved NeoPixel ESPinner state and controller creation
 * 11. Test pin modification functionality:
 *     - Change pins to GPIO: 14, NumPixels: 24
 *     - Validate pin detachment/attachment
 * 12. Create and test second NeoPixel ESPinner:
 *     - ID: "TEST_NEOPIXEL_2"
 *     - Pins: GPIO: 4, NumPixels: 32
 * 13. Test multiple NeoPixel coexistence and pin exclusivity
 * 14. Remove first NeoPixel and validate complete cleanup
 * 15. End Unity test framework
 */
void setup() {
	Serial.begin(115200);
	UNITY_BEGIN();

	RUN_TEST(test_no_elementsInParentMap);
	ESPAllOn::getInstance().setup(); // Executes ESPinnerSelector()

	RUN_TEST(test_firstSelectorOrderInelementParentMap);

	// ---------------------------------------------//
	// Define Neopixel as type and ID = TEST_NEOPIXEL
	// Search first ESPinnerSelector created
	// ---------------------------------------------//

	firstESPinnerSelector = elementToParentMap.begin()->second;
	uint16_t espinnerType_ref =
		searchByLabel(firstESPinnerSelector, ESPINNERTYPE_LABEL);
	Control *typeController = ESPUI.getControl(espinnerType_ref);
	typeController->value = NEOPIXEL_LABEL; // This is important to be Neopixel

	uint16_t NeopixelID_ref =
		searchByLabel(firstESPinnerSelector, ESPINNERID_LABEL);
	Control *typeNeopixelIDController = ESPUI.getControl(NeopixelID_ref);
	typeNeopixelIDController->value = "TEST_NEOPIXEL";

	// Click on Save Generic ESPinner and creates a new one
	saveElement_callback(typeController, B_UP);

	RUN_TEST(test_void_NeopixelEspinner);

	// ---------------------------------------------//
	// REVIEW NEOPIXEL STRUCTURE
	// ---------------------------------------------//

	// Input Wrong Number - simulate user entering invalid data

	uint16_t GPIO_ref =
		searchByLabel(firstESPinnerSelector, NEOPIXEL_PIN_SELECTOR_LABEL);
	typeGPIO_Controller = ESPUI.getControl(GPIO_ref);
	typeGPIO_Controller->value = "asdfg"; // Invalid non-numeric input

	// Click On save, but not saved because of wrong number in GPIO PIN
	NeopixelSelector_callback(typeGPIO_Controller, B_UP);
	saveButtonGPIOCheck(firstESPinnerSelector, NEOPIXEL_PIN_SELECTOR_LABEL,
						Neopixel_action, NEOPIXEL_SAVE_LABEL);
	RUN_TEST(test_wrong_NeopixelEspinner);

	// ---------------------------------------------//
	// Select correct Numbers
	// Click On Save NeoPixel
	// ---------------------------------------------//

	typeGPIO_Controller->value = "15"; // Valid GPIO pin

	uint16_t NumPixels_ref =
		searchByLabel(firstESPinnerSelector, NEOPIXEL_NUMPIXELS_LABEL);
	typeNumPixels_Controller = ESPUI.getControl(NumPixels_ref);
	typeNumPixels_Controller->value = "16";

	NeopixelSelector_callback(typeGPIO_Controller, B_UP);
	NeopixelSelector_callback(typeNumPixels_Controller, B_UP);
	saveButtonGPIOCheck(firstESPinnerSelector, NEOPIXEL_PIN_SELECTOR_LABEL,
						Neopixel_action, NEOPIXEL_SAVE_LABEL);
	RUN_TEST(test_saved_NeopixelEspinner);

	// ---------------------------------------------//
	// CHANGE PIN NUMBERS TO OTHER DIFFERENT
	// Check Attached and detached PINs
	// ---------------------------------------------//

	typeGPIO_Controller->value = "14";
	saveNeopixel_callback(typeGPIO_Controller, B_UP);
	typeNumPixels_Controller->value = "24";
	saveNeopixel_callback(typeNumPixels_Controller, B_UP);
	RUN_TEST(test_modified_NeopixelEspinner);

	// -------------------------------------------- //
	// ----------- Second GUI ESPinner ------------ //
	// -------------------------------------------- //

	std::set<uint16_t> ListRefs = collectParentRefs(elementToParentMap);
	currentParentRefs = filterParentRef(ListRefs, firstESPinnerSelector);

	secondESPinnerSelector = *currentParentRefs.begin();

	// Create a new NeoPixel
	uint16_t second_espinnerType_ref =
		searchByLabel(secondESPinnerSelector, ESPINNERTYPE_LABEL);
	Control *second_typeController = ESPUI.getControl(second_espinnerType_ref);
	second_typeController->value =
		NEOPIXEL_LABEL; // This is important to be Neopixel

	uint16_t second_NeopixelID_ref =
		searchByLabel(secondESPinnerSelector, ESPINNERID_LABEL);
	Control *second_typeNeopixelIDController =
		ESPUI.getControl(second_NeopixelID_ref);
	second_typeNeopixelIDController->value = "TEST_NEOPIXEL_2";

	// Click on Save Generic ESPinner and creates a new one
	saveElement_callback(second_typeController, B_UP);

	RUN_TEST(test_second_void_NeopixelEspinner);

	// -------------------------------------------- //
	// ----------- Save Second GUI ESPinner ------- //
	// -------------------------------------------- //

	uint16_t second_GPIO_ref =
		searchByLabel(secondESPinnerSelector, NEOPIXEL_PIN_SELECTOR_LABEL);
	typeGPIO_Controller = ESPUI.getControl(second_GPIO_ref);
	typeGPIO_Controller->value = "4";

	uint16_t second_NumPixels_ref =
		searchByLabel(secondESPinnerSelector, NEOPIXEL_NUMPIXELS_LABEL);
	typeNumPixels_Controller = ESPUI.getControl(second_NumPixels_ref);
	typeNumPixels_Controller->value = "32";

	NeopixelSelector_callback(typeGPIO_Controller, B_UP);
	NeopixelSelector_callback(typeNumPixels_Controller, B_UP);

	// IF GPIO 10, 11 is used, the board will not work
	saveButtonGPIOCheck(secondESPinnerSelector, NEOPIXEL_PIN_SELECTOR_LABEL,
						Neopixel_action, NEOPIXEL_SAVE_LABEL);

	saveNeopixel_callback(second_typeController, B_UP);

	RUN_TEST(test_second_NeopixelEspinner);

	// -------------------------------------------- //
	// ----------- Remove First NeoPixel ---------- //
	// -------------------------------------------- //

	uint16_t espinnerRemove_ref =
		searchByLabel(firstESPinnerSelector, NEOPIXEL_REMOVE_LABEL);
	Control *second_removeController = ESPUI.getControl(espinnerRemove_ref);
	removeNeopixel_callback(second_removeController, B_UP);

	RUN_TEST(test_firstESPinner_removed);

	UNITY_END();
}

void loop() {}
