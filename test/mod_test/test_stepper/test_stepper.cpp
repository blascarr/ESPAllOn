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

bool compareESPinnerStepper(const ESPinner_Stepper &expected_ESPinner_Stepper,
							const ESPinner_Stepper &actual_ESPinner_Stepper) {
	return expected_ESPinner_Stepper.STEP == actual_ESPinner_Stepper.STEP &&
		   expected_ESPinner_Stepper.DIR == actual_ESPinner_Stepper.DIR &&
		   expected_ESPinner_Stepper.EN == actual_ESPinner_Stepper.EN &&
		   expected_ESPinner_Stepper.ID == actual_ESPinner_Stepper.ID;
}

void loadStorage(const ESPinner_Stepper expectedList[]) {
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
			ESPinner_Stepper espinner;
			String output;
			serializeJson(obj, output);
			espinner.deserializeJSON(output);

			TEST_ASSERT_TRUE(
				compareESPinnerStepper(expectedList[espinner_index], espinner));
			espinner_index++;
		}
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
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(0, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());
}

void test_saved_StepperEspinner() {
	std::vector<ControlAssertion> saved_Stepper_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "Stepper"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_STEPPER"},
		{firstESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL, "14"},
		{firstESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL, "15"},
		{firstESPinnerSelector, STEPPER_EN_SELECTOR_LABEL, "16"},
		{firstESPinnerSelector, STEPPER_SAVE_LABEL, STEPPER_SAVE_VALUE},
		{firstESPinnerSelector, STEPPER_REMOVE_LABEL, STEPPER_REMOVE_VALUE}};

	assertControlValues(saved_Stepper_assertions);
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
	expected_espinner.setDIR(15);
	expected_espinner.setEN(16);
	expected_espinner.setID("TEST_STEPPER");
	ESPinner_Stepper expectedList[] = {expected_espinner};

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
	loadStorage(expectedList);
}

void test_modified_StepperEspinner() {
	std::vector<ControlAssertion> modified_Stepper_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "Stepper"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_STEPPER"},
		{firstESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL, "17"},
		{firstESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL, "18"},
		{firstESPinnerSelector, STEPPER_EN_SELECTOR_LABEL, "19"},
		{firstESPinnerSelector, STEPPER_SAVE_LABEL, STEPPER_SAVE_VALUE},
		{firstESPinnerSelector, STEPPER_REMOVE_LABEL, STEPPER_REMOVE_VALUE}};

	assertControlValues(modified_Stepper_assertions);
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
	ESPinner_Stepper expectedList[] = {expected_espinner};

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
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(14));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(15));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(16));

	// ----- Controllers Review ------ //
	TEST_ASSERT_EQUAL(
		3, ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	uint16_t parentRef = getParentId(elementToParentMap, firstESPinnerSelector);
	uint16_t controllerDIRRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_SWITCH_DIR_LABEL);
	uint16_t controllerRUNRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_SWITCH_RUN_LABEL);
	uint16_t controllerVELRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_SLIDER_VEL_LABEL);

	TEST_ASSERT_EQUAL(parentRef,
					  ESPinner_Manager::getInstance().findRefByControllerId(
						  controllerDIRRef));
	TEST_ASSERT_EQUAL(parentRef,
					  ESPinner_Manager::getInstance().findRefByControllerId(
						  controllerRUNRef));
	TEST_ASSERT_EQUAL(parentRef,
					  ESPinner_Manager::getInstance().findRefByControllerId(
						  controllerVELRef));

	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //
	loadStorage(expectedList);
}

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
		3, ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());
}

void test_second_StepperEspinner() {
	std::vector<ControlAssertion> void_Stepper_assertions = {
		{secondESPinnerSelector, ESPINNERTYPE_LABEL, "Stepper"},
		{secondESPinnerSelector, ESPINNERID_LABEL, "TEST_STEPPER_2"},
		{secondESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL, "21"},
		{secondESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL, "22"},
		{secondESPinnerSelector, STEPPER_EN_SELECTOR_LABEL, "23"},
		{secondESPinnerSelector, STEPPER_SAVE_LABEL, STEPPER_SAVE_VALUE},
		{secondESPinnerSelector, STEPPER_REMOVE_LABEL, STEPPER_REMOVE_VALUE}};

	assertControlValues(void_Stepper_assertions);

	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(2, EspinnerListSize);

	TEST_ASSERT_EQUAL_INT16(3, controlReferences.size());

	ESPinner_Stepper first_espinner = ESPinner_Stepper();
	first_espinner.setSTEP(17);
	first_espinner.setDIR(18);
	first_espinner.setEN(19);
	first_espinner.setID("TEST_STEPPER");

	ESPinner_Stepper expected_espinner = ESPinner_Stepper();
	expected_espinner.setSTEP(21);
	expected_espinner.setDIR(22);
	expected_espinner.setEN(23);
	expected_espinner.setID("TEST_STEPPER_2");

	ESPinner_Stepper expectedList[] = {first_espinner, expected_espinner};

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
	TEST_ASSERT_EQUAL(
		6, ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		2, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	uint16_t parentRef =
		getParentId(elementToParentMap, secondESPinnerSelector);
	uint16_t controllerDIRRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_SWITCH_DIR_LABEL);
	uint16_t controllerRUNRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_SWITCH_RUN_LABEL);
	uint16_t controllerVELRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_SLIDER_VEL_LABEL);

	TEST_ASSERT_EQUAL(parentRef,
					  ESPinner_Manager::getInstance().findRefByControllerId(
						  controllerDIRRef));
	TEST_ASSERT_EQUAL(parentRef,
					  ESPinner_Manager::getInstance().findRefByControllerId(
						  controllerRUNRef));
	TEST_ASSERT_EQUAL(parentRef,
					  ESPinner_Manager::getInstance().findRefByControllerId(
						  controllerVELRef));

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

	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());

	ESPinner_Stepper expected_espinner = ESPinner_Stepper();
	expected_espinner.setSTEP(21);
	expected_espinner.setDIR(22);
	expected_espinner.setEN(23);
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

	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(17));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(18));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(19));

	TEST_ASSERT_EQUAL(
		3, ESPinner_Manager::getInstance().getControllerMap().size());

	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //
	ESPinner_Stepper expectedList[] = {expected_espinner};
	loadStorage(expectedList);
}

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

	// Input Wrong Number
	uint16_t STEP_ref =
		searchByLabel(firstESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL);
	typeSTEP_Controller = ESPUI.getControl(STEP_ref);
	typeSTEP_Controller->value = "asdfg";

	// Click On save, but not saved because of wrong number in STEP PIN
	StepperSelector_callback(typeSTEP_Controller, B_UP);
	saveButtonGPIOCheck(firstESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL,
						Stepper_action);
	RUN_TEST(test_wrong_StepperEspinner);

	// ---------------------------------------------//
	// Select correct Numbers
	// Click On Save Stepper
	// ---------------------------------------------//
	typeSTEP_Controller->value = "14";

	uint16_t DIR_ref =
		searchByLabel(firstESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL);
	typeDIR_Controller = ESPUI.getControl(DIR_ref);
	typeDIR_Controller->value = "15";

	uint16_t EN_ref =
		searchByLabel(firstESPinnerSelector, STEPPER_EN_SELECTOR_LABEL);
	typeEN_Controller = ESPUI.getControl(EN_ref);
	typeEN_Controller->value = "16";

	StepperSelector_callback(typeSTEP_Controller, B_UP);
	StepperSelector_callback(typeDIR_Controller, B_UP);
	StepperSelector_callback(typeEN_Controller, B_UP);

	saveButtonGPIOCheck(firstESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL,
						Stepper_action);
	saveButtonGPIOCheck(firstESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL,
						Stepper_action);
	saveButtonGPIOCheck(firstESPinnerSelector, STEPPER_EN_SELECTOR_LABEL,
						Stepper_action);

	RUN_TEST(test_saved_StepperEspinner);

	// ---------------------------------------------//
	// CHANGE PIN NUMBERS TO OTHER DIFFERENT
	// Check Attached and detached PINs
	// ---------------------------------------------//
	typeSTEP_Controller->value = "17";
	saveStepper_callback(typeSTEP_Controller, B_UP);
	typeDIR_Controller->value = "18";
	saveStepper_callback(typeDIR_Controller, B_UP);
	typeEN_Controller->value = "19";
	saveStepper_callback(typeEN_Controller, B_UP);

	RUN_TEST(test_modified_StepperEspinner);

	// -------------------------------------------- //
	// ----------- Save Second VOID GUI ESPinner ------------ //
	// -------------------------------------------- //

	std::set<uint16_t> ListRefs = collectParentRefs(elementToParentMap);
	currentParentRefs = filterParentRef(ListRefs, firstESPinnerSelector);

	secondESPinnerSelector = *currentParentRefs.begin();

	uint16_t second_STEPPER_TYPE_ref =
		searchByLabel(secondESPinnerSelector, ESPINNERTYPE_LABEL);
	Control *second_typeSTEPPERController =
		ESPUI.getControl(second_STEPPER_TYPE_ref);
	second_typeSTEPPERController->value = STEPPER_LABEL;

	uint16_t second_STEPPER_ID_ref =
		searchByLabel(secondESPinnerSelector, ESPINNERID_LABEL);
	Control *second_STEPPERIDController =
		ESPUI.getControl(second_STEPPER_ID_ref);
	second_STEPPERIDController->value = "TEST_STEPPER_2";

	saveElement_callback(second_typeSTEPPERController, B_UP);
	RUN_TEST(test_second_void_StepperEspinner);

	// -------------------------------------------- //
	// ----------- Save Second GUI ESPinner ------------ //
	// -------------------------------------------- //
	uint16_t second_STEP_ref =
		searchByLabel(secondESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL);
	typeSTEP_Controller = ESPUI.getControl(second_STEP_ref);
	typeSTEP_Controller->value = "21";

	uint16_t second_DIR_ref =
		searchByLabel(secondESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL);
	typeDIR_Controller = ESPUI.getControl(second_DIR_ref);
	typeDIR_Controller->value = "22";

	uint16_t second_EN_ref =
		searchByLabel(secondESPinnerSelector, STEPPER_EN_SELECTOR_LABEL);
	typeEN_Controller = ESPUI.getControl(second_EN_ref);
	typeEN_Controller->value = "23";

	StepperSelector_callback(typeSTEP_Controller, B_UP);
	StepperSelector_callback(typeEN_Controller, B_UP);

	saveButtonGPIOCheck(secondESPinnerSelector, STEPPER_STEP_SELECTOR_LABEL,
						Stepper_action);
	saveButtonGPIOCheck(secondESPinnerSelector, STEPPER_DIR_SELECTOR_LABEL,
						Stepper_action);
	saveButtonGPIOCheck(secondESPinnerSelector, STEPPER_EN_SELECTOR_LABEL,
						Stepper_action);

	saveStepper_callback(second_typeSTEPPERController, B_UP);
	RUN_TEST(test_second_StepperEspinner);

	// -------------------------------------------- //
	// ----------- Remove First Stepper ------------ //
	// -------------------------------------------- //

	uint16_t espinnerRemove_ref =
		searchByLabel(firstESPinnerSelector, STEPPER_REMOVE_LABEL);
	Control *second_removeController = ESPUI.getControl(espinnerRemove_ref);
	removeStepper_callback(second_removeController, B_UP);

	// RUN_TEST(test_firstESPinner_removed);

	UNITY_END();
}

void loop() {}