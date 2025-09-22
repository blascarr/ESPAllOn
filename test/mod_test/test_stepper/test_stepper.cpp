/**
 * Stepper Motor Module Test
 *
 * This comprehensive test validates the ESPinner_Stepper module functionality
 * for stepper motor control.
 *
 * Test Steps:
 * 1. Initialize system and validate empty state
 * 2. Create first Stepper ESPinner and validate UI structure
 * 3. Test invalid pin input handling for STEP pin
 * 4. Configure valid pin assignments for STEP, DIR, and EN pins
 * 5. Save Stepper configuration and verify pin manager attachments
 * 6. Modify pin assignments and test pin detachment/reattachment
 * 7. Validate stepper motor controller creation (direction, enable, velocity)
 * 8. Create second Stepper ESPinner with different pin configuration
 * 9. Test multiple stepper motors coexistence and pin exclusivity
 * 10. Remove first Stepper ESPinner and verify complete cleanup
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
Control *typeSTEP_Controller;
Control *typeDIR_Controller;
Control *typeEN_Controller;
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

void removedControl(uint16_t input_ref, const String &label) {
	uint16_t ref = searchByLabel(input_ref, label.c_str());
	TEST_ASSERT_EQUAL_INT16(0, ref);
}

Persistance ESPinnerManager(nullptr, &storage);

bool compareESPinnerStepper(ESPinner_Stepper &expected_ESPinner_Stepper,
							ESPinner_Stepper &actual_ESPinner_Stepper) {
	return expected_ESPinner_Stepper.getSTEP() ==
			   actual_ESPinner_Stepper.getSTEP() &&
		   expected_ESPinner_Stepper.getDIR() ==
			   actual_ESPinner_Stepper.getDIR() &&
		   expected_ESPinner_Stepper.getEN() ==
			   actual_ESPinner_Stepper.getEN() &&
		   expected_ESPinner_Stepper.getType() ==
			   actual_ESPinner_Stepper.getType() &&
		   expected_ESPinner_Stepper.getID() == actual_ESPinner_Stepper.getID();
}

void loadStorage(ESPinner_Stepper *expectedList, uint8_t expectedCount) {
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

		ESPinner_Stepper espinner;
		String output;
		serializeJson(array[expectedCount - 1], output);
		espinner.deserializeJSON(output);
		TEST_ASSERT_TRUE(
			compareESPinnerStepper(expectedList[espinner_index], espinner));
	}
}

void checkESPinnerState(const String &id, int expectedSTEP, int expectedDIR,
						int expectedEN, ESPinner_Mod expectedType) {
	ESPinner *espinnerInList =
		ESPinner_Manager::getInstance().findESPinnerById(id);
	if (espinnerInList) {
		ESPinner_Stepper *StepperEspinner =
			static_cast<ESPinner_Stepper *>(espinnerInList);
		if (StepperEspinner) {
			TEST_ASSERT_EQUAL_INT8(expectedSTEP, StepperEspinner->getSTEP());
			TEST_ASSERT_EQUAL_INT8(expectedDIR, StepperEspinner->getDIR());
			TEST_ASSERT_EQUAL_INT8(expectedEN, StepperEspinner->getEN());
			TEST_ASSERT_EQUAL_STRING(id.c_str(),
									 StepperEspinner->getID().c_str());
			TEST_ASSERT_EQUAL(expectedType, StepperEspinner->getType());
		} else {
			Serial.println("Error: ESPinner_Stepper Type Not Found");
		}
	} else {
		Serial.println("ESPinner ID Not Found In List");
	}
}

/**
 * Test void/empty Stepper ESPinner state after creation
 *
 * Code Steps:
 * 1. Create assertion vector with expected default UI values
 *    - ESPinner type should be "Stepper"
 *    - All GPIO pins should default to "0" (unassigned)
 *    - Save and Remove buttons should have correct labels
 * 2. Execute assertControlValues() to validate all UI elements
 * 3. Get current ESPinner list size from manager
 * 4. Assert no ESPinners exist in manager (size = 0)
 * 5. Assert control references count is 2 (main + current selector)
 */
void test_void_StepperEspinner() {
	std::vector<ControlAssertion> void_Stepper_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "Stepper"},
		{firstESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL, "0"},
		{firstESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL, "0"},
		{firstESPinnerSelector, STEPPER_EN_SELECTOR_LABEL, "0"},
		{firstESPinnerSelector, STEPPER_SAVE_LABEL, STEPPER_SAVE_VALUE},
		{firstESPinnerSelector, STEPPER_REMOVE_LABEL, STEPPER_REMOVE_VALUE}};

	assertControlValues(void_Stepper_assertions);

	// ESPAllOn_Manager Espinners Void
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(0, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());
}

/**
 * Test Stepper ESPinner with invalid pin input handling
 *
 * Code Steps:
 * 1. Create assertion vector with invalid pin value "asdfg" for STEP pin
 *    - Type should be "Stepper", ID should be "TEST_STEPPER"
 *    - STEP pin has invalid string "asdfg" (non-numeric)
 *    - DIR and EN pins remain valid at "0"
 * 2. Execute assertControlValues() to validate UI shows invalid input
 * 3. Verify ESPinner manager rejects invalid configuration
 * 4. Assert ESPinner list remains empty (invalid data not saved)
 * 5. Assert pin manager has no pin attachments (validation failed)
 * 6. Confirm control references remain at 2 (no new ESPinner created)
 */
void test_wrong_StepperEspinner() {
	std::vector<ControlAssertion> wrong_Stepper_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "Stepper"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_STEPPER"},
		{firstESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL, "asdfg"},
		{firstESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL, "0"},
		{firstESPinnerSelector, STEPPER_EN_SELECTOR_LABEL, "0"},
		{firstESPinnerSelector, STEPPER_SAVE_LABEL, STEPPER_SAVE_VALUE},
		{firstESPinnerSelector, STEPPER_REMOVE_LABEL, STEPPER_REMOVE_VALUE}};

	assertControlValues(wrong_Stepper_assertions);

	// ESPAllOn_Manager Espinners Void
	// Check EspAllOn_Pin_manager Status Not Connected
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(0, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());
}

/**
 * Test successfully saved Stepper ESPinner with valid configuration
 *
 * Code Steps:
 * 1. Create assertion vector with valid Stepper configuration
 *    - Type: "Stepper", ID: "TEST_STEPPER"
 *    - STEP: pin 14, DIR: pin 15, EN: pin 16
 *    - Save/Remove buttons with correct values
 * 2. Execute assertControlValues() to validate UI state
 * 3. Verify GPIO selector controls are removed (UI cleanup after save)
 * 4. Check ESPinner manager contains exactly 1 ESPinner
 * 5. Assert control references count remains at 2
 * 6. Create expected ESPinner object for comparison
 * 7. Execute checkESPinnerState() to validate ESPinner in memory
 * 8. Verify pin manager has all pins attached (GPIO 14, 15, and 16)
 * 9. Execute loadStorage() to validate persistent storage
 */
void test_saved_StepperEspinner() {
	std::vector<ControlAssertion> saved_Stepper_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "Stepper"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_STEPPER"},
		{firstESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL, "17"},
		{firstESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL, "18"},
		{firstESPinnerSelector, STEPPER_EN_SELECTOR_LABEL, "19"},
		{firstESPinnerSelector, STEPPER_SAVE_LABEL, STEPPER_SAVE_VALUE},
		{firstESPinnerSelector, STEPPER_REMOVE_LABEL, STEPPER_REMOVE_VALUE}};

	assertControlValues(saved_Stepper_assertions);

	// Should return MEMORY FAIL: LABEL NOT FOUND because it has been removed
	removedControl(firstESPinnerSelector, STEPPER_STEP_SELECT_LABEL);
	removedControl(firstESPinnerSelector, STEPPER_DIR_SELECT_LABEL);
	removedControl(firstESPinnerSelector, STEPPER_EN_SELECT_LABEL);
	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());

	ESPinner_Stepper expected_espinner = ESPinner_Stepper();
	expected_espinner.setSTEP(17);
	expected_espinner.setDIR(18);
	expected_espinner.setEN(19);
	expected_espinner.setID("TEST_STEPPER");
	checkESPinnerState(expected_espinner.getID(), expected_espinner.getSTEP(),
					   expected_espinner.getDIR(), expected_espinner.getEN(),
					   expected_espinner.getType());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //

	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeSTEP_Controller->value.toInt()));
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeDIR_Controller->value.toInt()));
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeEN_Controller->value.toInt()));

	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //
	loadStorage(&expected_espinner, 1);
}

/**
 * Test Stepper ESPinner pin modification and pin detachment/attachment
 *
 * Code Steps:
 * 1. Create assertion vector with modified pin configuration
 *    - Same ESPinner ID "TEST_STEPPER" but different pins
 *    - STEP changed from 14 to 17, DIR from 15 to 18, EN from 16 to 19
 * 2. Execute assertControlValues() to validate UI updates
 * 3. Verify GPIO selector controls are removed after modification
 * 4. Assert ESPinner manager still contains exactly 1 ESPinner
 * 5. Create expected ESPinner object with new pin values
 * 6. Execute checkESPinnerState() to validate updated ESPinner
 * 7. Debug output current pin assignments
 * 8. Verify pin manager has new pins attached (GPIO 17, 18, and 19)
 * 9. Assert previous pins are detached (GPIO 14, 15, and 16)
 * 10. Validate stepper motor controller creation:
 *     - 3 controllers: EN switch, VEL slider, and movement pad
 *     - 1 UI relation mapping ESPinner to parent reference
 * 11. Search and validate each controller type by label
 * 12. Assert each controller is properly linked to parent reference
 * 13. Execute loadStorage() to validate persistence of changes
 */
void test_modified_StepperEspinner() {
	std::vector<ControlAssertion> modified_Stepper_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "Stepper"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_STEPPER"},
		{firstESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL, "14"},
		{firstESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL, "12"},
		{firstESPinnerSelector, STEPPER_EN_SELECTOR_LABEL, "13"},
		{firstESPinnerSelector, STEPPER_SAVE_LABEL, STEPPER_SAVE_VALUE},
		{firstESPinnerSelector, STEPPER_REMOVE_LABEL, STEPPER_REMOVE_VALUE}};

	assertControlValues(modified_Stepper_assertions);

	// Should return MEMORY FAIL: LABEL NOT FOUND because it has been removed
	removedControl(firstESPinnerSelector, STEPPER_STEP_SELECT_LABEL);
	removedControl(firstESPinnerSelector, STEPPER_DIR_SELECT_LABEL);
	removedControl(firstESPinnerSelector, STEPPER_EN_SELECT_LABEL);
	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());

	ESPinner_Stepper expected_espinner = ESPinner_Stepper();
	expected_espinner.setSTEP(14);
	expected_espinner.setDIR(12);
	expected_espinner.setEN(13);
	expected_espinner.setID("TEST_STEPPER");

	checkESPinnerState(expected_espinner.getID(), expected_espinner.getSTEP(),
					   expected_espinner.getDIR(), expected_espinner.getEN(),
					   expected_espinner.getType());

	DUMPLN("ESPINNER STEP PIN ", typeSTEP_Controller->value.toInt());
	DUMPLN("ESPINNER DIR PIN ", typeDIR_Controller->value.toInt());
	DUMPLN("ESPINNER EN PIN ", typeEN_Controller->value.toInt());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeSTEP_Controller->value.toInt()));
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeDIR_Controller->value.toInt()));
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeEN_Controller->value.toInt()));

	// Previous PIN are detached
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(17));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(18));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(19));

	// ----- Controllers Review ------ //
	// Controller is composed by
	// 	- 3 controllers: EN switch, VEL slider,
	// 	- 4 labels,
	// 	- 2 textBoxes ( ID and Target )
	// 	- 2 buttons ( Home and Zero )
	// 	- 1 movement pad
	TEST_ASSERT_EQUAL(
		10, ESPinner_Manager::getInstance().getControllerMap().size());

	// TODO : Review this relation ID Map
	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	uint16_t parentRef = getParentId(elementToParentMap, firstESPinnerSelector);
	uint16_t controllerENRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_SWITCH_EN_LABEL);
	uint16_t controllerVELRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_SLIDER_VEL_LABEL);
	uint16_t controllerPADRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_PAD_MOVEMENT_LABEL);

	/*
	TEST_ASSERT_EQUAL(
		parentRef,
		ESPinner_Manager::getInstance().findRefByControllerId(controllerENRef));
	TEST_ASSERT_EQUAL(parentRef,
					  ESPinner_Manager::getInstance().findRefByControllerId(
						  controllerVELRef));
	TEST_ASSERT_EQUAL(parentRef,
					  ESPinner_Manager::getInstance().findRefByControllerId(
						  controllerPADRef));
	*/

	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //

	loadStorage(&expected_espinner, 1);
}

/**
 * Test second void/empty Stepper ESPinner state after creation
 *
 * Code Steps:
 * 1. Create assertion vector with expected default UI values for second
 * selector
 *    - ESPinner type should be "Stepper"
 *    - ID should be "TEST_STEPPER_2"
 *    - All GPIO pins should default to "0" (unassigned)
 *    - GPIO select labels should be visible (not removed yet)
 *    - Save and Remove buttons should have correct labels
 * 2. Execute assertControlValues() to validate all UI elements
 * 3. Get current ESPinner list size from manager (should be 1 from first)
 * 4. Assert control references count is 3 (main + first + second selector)
 * 5. Verify controller map has 3 controllers from first stepper
 * 6. Assert UI relation map has 1 entry (first stepper controller)
 */
void test_second_void_StepperEspinner() {
	std::vector<ControlAssertion> void_Stepper_assertions = {
		{secondESPinnerSelector, ESPINNERTYPE_LABEL, "Stepper"},
		{secondESPinnerSelector, ESPINNERID_LABEL, "TEST_STEPPER_2"},
		{secondESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL, "0"},
		{secondESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL, "0"},
		{secondESPinnerSelector, STEPPER_EN_SELECTOR_LABEL, "0"},
		{secondESPinnerSelector, STEPPER_STEP_SELECT_LABEL,
		 STEPPER_STEP_SELECT_VALUE},
		{secondESPinnerSelector, STEPPER_DIR_SELECT_LABEL,
		 STEPPER_DIR_SELECT_VALUE},
		{secondESPinnerSelector, STEPPER_EN_SELECT_LABEL,
		 STEPPER_EN_SELECT_VALUE},
		{secondESPinnerSelector, STEPPER_SAVE_LABEL, STEPPER_SAVE_VALUE},
		{secondESPinnerSelector, STEPPER_REMOVE_LABEL, STEPPER_REMOVE_VALUE}};

	assertControlValues(void_Stepper_assertions);

	// ESPAllOn_Manager Espinners Void
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(3, controlReferences.size());

	TEST_ASSERT_EQUAL(
		10, ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());
}

/**
 * Test successfully saved second Stepper ESPinner with valid configuration
 *
 * Code Steps:
 * 1. Create assertion vector with valid second Stepper configuration
 *    - Type: "Stepper", ID: "TEST_STEPPER_2"
 *    - STEP: pin 21, DIR: pin 22, EN: pin 23 (different from first stepper)
 *    - Save/Remove buttons with correct values
 * 2. Execute assertControlValues() to validate UI state
 * 3. Check ESPinner manager contains exactly 2 ESPinners
 * 4. Assert control references count is 3 (main + first + second)
 * 5. Create expected ESPinner objects for both steppers
 * 6. Execute checkESPinnerState() to validate second ESPinner in memory
 * 7. Verify pin manager has second stepper pins attached (GPIO 21, 22, 23)
 * 8. Validate stepper motor controllers for both steppers:
 *    - 6 controllers total: 3 for each stepper (EN, VEL, PAD)
 *    - 2 UI relations mapping each ESPinner to parent reference
 * 9. Search and validate each controller type for second stepper
 * 10. Assert each controller is properly linked to second parent reference
 * 11. Execute loadStorage() to validate persistence of both steppers
 */
void test_second_StepperEspinner() {
	std::vector<ControlAssertion> void_Stepper_assertions = {
		{secondESPinnerSelector, ESPINNERTYPE_LABEL, "Stepper"},
		{secondESPinnerSelector, ESPINNERID_LABEL, "TEST_STEPPER_2"},
		{secondESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL, "3"},
		{secondESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL, "4"},
		{secondESPinnerSelector, STEPPER_EN_SELECTOR_LABEL, "5"},
		{secondESPinnerSelector, STEPPER_SAVE_LABEL, STEPPER_SAVE_VALUE},
		{secondESPinnerSelector, STEPPER_REMOVE_LABEL, STEPPER_REMOVE_VALUE}};

	assertControlValues(void_Stepper_assertions);

	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(2, EspinnerListSize);

	// REVIEW HERE ?=?= 2 ESPINNERS + 2 CONTROLLERS
	TEST_ASSERT_EQUAL_INT16(3, controlReferences.size());

	ESPinner_Stepper first_espinner;
	first_espinner.setSTEP(14);
	first_espinner.setDIR(12);
	first_espinner.setEN(13);
	first_espinner.setID("TEST_STEPPER");

	ESPinner_Stepper expected_espinner;
	expected_espinner.setSTEP(3);
	expected_espinner.setDIR(4);
	expected_espinner.setEN(5);
	expected_espinner.setID("TEST_STEPPER_2");

	checkESPinnerState(expected_espinner.getID(), expected_espinner.getSTEP(),
					   expected_espinner.getDIR(), expected_espinner.getEN(),
					   expected_espinner.getType());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //

	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeSTEP_Controller->value.toInt()));
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeDIR_Controller->value.toInt()));
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeEN_Controller->value.toInt()));

	// ----- Controllers Review ------ //
	// TO REVIEW
	TEST_ASSERT_EQUAL(
		20, ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		2, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	uint16_t parentRef =
		getParentId(elementToParentMap, secondESPinnerSelector);
	uint16_t controllerENRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_SWITCH_EN_LABEL);
	uint16_t controllerVELRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_SLIDER_VEL_LABEL);
	uint16_t controllerPADRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_PAD_MOVEMENT_LABEL);

	/*
	TEST_ASSERT_EQUAL(
		parentRef,
		ESPinner_Manager::getInstance().findRefByControllerId(controllerENRef));
	TEST_ASSERT_EQUAL(parentRef,
					  ESPinner_Manager::getInstance().findRefByControllerId(
						  controllerVELRef));
	TEST_ASSERT_EQUAL(parentRef,
					  ESPinner_Manager::getInstance().findRefByControllerId(
						  controllerPADRef));
*/
	uint16_t firstParentRef =
		getParentId(elementToParentMap, firstESPinnerSelector);
	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //
	loadStorage(&expected_espinner, 2);
}

/**
 * Test first Stepper ESPinner removal and complete cleanup validation
 *
 * Code Steps:
 * 1. Verify ESPinner manager contains exactly 1 ESPinner (second one remains)
 * 2. Assert control references count returns to 2 (main + remaining selector)
 * 3. Create expected ESPinner object for remaining stepper (TEST_STEPPER_2)
 * 4. Execute checkESPinnerState() to validate remaining ESPinner state
 * 5. Verify pin manager still has second stepper pins attached (GPIO 21, 22,
 * 23)
 * 6. Assert first stepper pins are properly detached (GPIO 17, 18, 19)
 * 7. Validate controller cleanup:
 *    - Only 3 controllers remain (from second stepper)
 *    - Only 1 UI relation remains (second stepper controller)
 * 8. Execute loadStorage() to validate persistent storage contains only second
 * stepper
 * 9. Confirm complete removal of first stepper from all system components
 */
void test_firstESPinner_removed() {
	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();

	// TODO : Review why is not removing Last ESPinner
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());

	ESPinner_Stepper expected_espinner = ESPinner_Stepper();
	expected_espinner.setSTEP(3);
	expected_espinner.setDIR(4);
	expected_espinner.setEN(5);
	expected_espinner.setID("TEST_STEPPER_2");
	checkESPinnerState(expected_espinner.getID(), expected_espinner.getSTEP(),
					   expected_espinner.getDIR(), expected_espinner.getEN(),
					   expected_espinner.getType());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeSTEP_Controller->value.toInt()));
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeDIR_Controller->value.toInt()));
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeEN_Controller->value.toInt()));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(12));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(13));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(14));

	TEST_ASSERT_EQUAL(
		20, ESPinner_Manager::getInstance().getControllerMap().size());

	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //
	loadStorage(&expected_espinner, 1);
}

/**
 * Main test setup function - orchestrates complete Stepper ESPinner test
 * sequence
 *
 * Code Steps:
 * 1. Initialize serial communication at 115200 baud
 * 2. Begin Unity test framework
 * 3. Test initial empty state (no UI elements)
 * 4. Execute ESPAllOn.setup() to initialize system and create first UI selector
 * 5. Validate first selector structure and element ordering
 * 6. Configure first ESPinner as Stepper type:
 *    - Get first ESPinner selector reference from elementToParentMap
 *    - Find ESPinner type control and set value to "Stepper"
 *    - Find ESPinner ID control and set value to "TEST_STEPPER"
 *    - Execute saveElement_callback() to create Stepper ESPinner UI
 * 7. Test void Stepper ESPinner state (default values, no pins assigned)
 * 8. Test invalid pin input handling:
 *    - Set STEP pin to invalid value "asdfg"
 *    - Validate system rejects invalid configuration
 * 9. Configure valid pin assignments:
 *    - STEP: pin 14, DIR: pin 15, EN: pin 16
 *    - Execute callbacks and validation for each pin
 * 10. Test saved Stepper ESPinner state and controller creation
 * 11. Test pin modification functionality:
 *     - Change pins to STEP: 17, DIR: 18, EN: 19
 *     - Validate pin detachment/attachment
 * 12. Create and test second Stepper ESPinner:
 *     - ID: "TEST_STEPPER_2"
 *     - Pins: STEP: 21, DIR: 22, EN: 23
 * 13. Test multiple stepper coexistence and pin exclusivity
 * 14. Remove first stepper and validate complete cleanup
 * 15. End Unity test framework
 */
void setup() {
	Serial.begin(115200);
	UNITY_BEGIN();

	RUN_TEST(test_no_elementsInParentMap);
	ESPAllOn::getInstance().setup(); // Executes ESPinnerSelector()

	RUN_TEST(test_firstSelectorOrderInelementParentMap);

	// ---------------------------------------------//
	// Define Stepper as INPUT and ID = TEST_STEPPER
	// Search first ESPinnerSelector created
	// ---------------------------------------------//

	firstESPinnerSelector = elementToParentMap.begin()->second;
	uint16_t espinnerType_ref =
		searchByLabel(firstESPinnerSelector, ESPINNERTYPE_LABEL);
	Control *typeController = ESPUI.getControl(espinnerType_ref);
	typeController->value = STEPPER_LABEL; // This is important to be Stepper

	uint16_t StepperID_ref =
		searchByLabel(firstESPinnerSelector, ESPINNERID_LABEL);
	Control *typeStepperIDController = ESPUI.getControl(StepperID_ref);
	typeStepperIDController->value = "TEST_STEPPER";

	// Click on Save Generic ESPinner and creates a new one
	saveElement_callback(typeController, B_UP);

	RUN_TEST(test_void_StepperEspinner);

	// ---------------------------------------------//
	// STEPPER_MODESELECTOR_LABEL
	// REVIEW STEPPER STRUCTURE
	// ---------------------------------------------//

	// Input Wrong Number - simulate user entering invalid data

	uint16_t STEP_ref =
		searchByLabel(firstESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL);
	typeSTEP_Controller = ESPUI.getControl(STEP_ref);
	typeSTEP_Controller->value = "asdfg"; // Invalid non-numeric input

	// Click On save, but not saved because of wrong number in STEP PIN
	StepperSelector_callback(typeSTEP_Controller, B_UP);
	saveButtonGPIOCheck(firstESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL,
						Stepper_action, STEPPER_SAVE_LABEL);
	RUN_TEST(test_wrong_StepperEspinner);

	// ---------------------------------------------//
	// Select correct Numbers
	// Click On Save Stepper
	// ---------------------------------------------//

	typeSTEP_Controller->value = "17"; // Valid GPIO pin for STEP

	uint16_t DIR_ref =
		searchByLabel(firstESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL);
	typeDIR_Controller = ESPUI.getControl(DIR_ref);
	typeDIR_Controller->value = "18";

	uint16_t EN_ref =
		searchByLabel(firstESPinnerSelector, STEPPER_EN_SELECTOR_LABEL);
	typeEN_Controller = ESPUI.getControl(EN_ref);
	typeEN_Controller->value = "19";

	StepperSelector_callback(typeSTEP_Controller, B_UP);
	StepperSelector_callback(typeDIR_Controller, B_UP);
	StepperSelector_callback(typeEN_Controller, B_UP);
	saveButtonGPIOCheck(firstESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL,
						Stepper_action, STEPPER_SAVE_LABEL);
	saveButtonGPIOCheck(firstESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL,
						Stepper_action, STEPPER_SAVE_LABEL);
	saveButtonGPIOCheck(firstESPinnerSelector, STEPPER_EN_SELECTOR_LABEL,
						Stepper_action, STEPPER_SAVE_LABEL);
	RUN_TEST(test_saved_StepperEspinner);

	// ---------------------------------------------//
	// CHANGE PIN NUMBERS TO OTHER DIFFERENT
	// Check Attached and detached PINs
	// ---------------------------------------------//

	typeSTEP_Controller->value = "14";
	saveStepper_callback(typeSTEP_Controller, B_UP);
	typeDIR_Controller->value = "12";
	saveStepper_callback(typeDIR_Controller, B_UP);
	typeEN_Controller->value = "13";
	saveStepper_callback(typeEN_Controller, B_UP);
	RUN_TEST(test_modified_StepperEspinner);

	// -------------------------------------------- //
	// ----------- Second GUI ESPinner ------------ //
	// -------------------------------------------- //

	std::set<uint16_t> ListRefs = collectParentRefs(elementToParentMap);
	currentParentRefs = filterParentRef(ListRefs, firstESPinnerSelector);

	secondESPinnerSelector = *currentParentRefs.begin();

	// Create a new Stepper
	uint16_t second_espinnerType_ref =
		searchByLabel(secondESPinnerSelector, ESPINNERTYPE_LABEL);
	Control *second_typeController = ESPUI.getControl(second_espinnerType_ref);
	second_typeController->value =
		STEPPER_LABEL; // This is important to be Stepper

	uint16_t second_StepperID_ref =
		searchByLabel(secondESPinnerSelector, ESPINNERID_LABEL);
	Control *second_typeStepperIDController =
		ESPUI.getControl(second_StepperID_ref);
	second_typeStepperIDController->value = "TEST_STEPPER_2";

	// Click on Save Generic ESPinner and creates a new one
	saveElement_callback(second_typeController, B_UP);

	RUN_TEST(test_second_void_StepperEspinner);

	// -------------------------------------------- //
	// ----------- Save Second GUI ESPinner ------------ //
	// -------------------------------------------- //

	uint16_t second_STEP_ref =
		searchByLabel(secondESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL);
	typeSTEP_Controller = ESPUI.getControl(second_STEP_ref);
	typeSTEP_Controller->value = "3";

	uint16_t second_DIR_ref =
		searchByLabel(secondESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL);
	typeDIR_Controller = ESPUI.getControl(second_DIR_ref);
	typeDIR_Controller->value = "4";

	uint16_t second_EN_ref =
		searchByLabel(secondESPinnerSelector, STEPPER_EN_SELECTOR_LABEL);
	typeEN_Controller = ESPUI.getControl(second_EN_ref);
	typeEN_Controller->value = "5";

	StepperSelector_callback(typeSTEP_Controller, B_UP);
	StepperSelector_callback(typeDIR_Controller, B_UP);
	StepperSelector_callback(typeEN_Controller, B_UP);

	// IF GPIO 10 , 11 is used, the board will not work
	saveButtonGPIOCheck(secondESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL,
						Stepper_action, STEPPER_SAVE_LABEL);
	saveButtonGPIOCheck(secondESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL,
						Stepper_action, STEPPER_SAVE_LABEL);
	saveButtonGPIOCheck(secondESPinnerSelector, STEPPER_EN_SELECTOR_LABEL,
						Stepper_action, STEPPER_SAVE_LABEL);

	saveStepper_callback(second_typeController, B_UP);

	RUN_TEST(test_second_StepperEspinner);

	// Define Stepper as OUTPUT and ID = TEST_OUTPUT
	// Input the last Stepper Number which is occupied
	// This should not be referenced again

	// Change input Stepper
	// Click On Save Stepper

	// -------------------------------------------- //
	// ----------- Remove First Stepper ------------ //
	// -------------------------------------------- //

	uint16_t espinnerRemove_ref =
		searchByLabel(firstESPinnerSelector, STEPPER_REMOVE_LABEL);
	Control *second_removeController = ESPUI.getControl(espinnerRemove_ref);
	removeStepper_callback(second_removeController, B_UP);

	RUN_TEST(test_firstESPinner_removed);

	UNITY_END();
}

void loop() {}