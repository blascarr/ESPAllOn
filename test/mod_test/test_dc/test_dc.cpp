
#include <Arduino.h>
#include <config.h>
#include <set>
#include <utils.h>

#include "../../config.h"
#include "../../utils/testTicker.h"

std::vector<std::string> expectedLabels = {ESPINNERTYPE_LABEL, ESPINNERID_LABEL,
										   SAVEESPINNER_LABEL};
uint16_t firstESPinnerSelector;
Control *typeDC_AController;
Control *typeDC_BController;
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

bool compareESPinnerDC(const ESPinner_DC &expected_ESPinner_DC,
					   const ESPinner_DC &actual_ESPinner_DC) {
	return expected_ESPinner_DC.gpioA == actual_ESPinner_DC.gpioA &&
		   expected_ESPinner_DC.gpioB == actual_ESPinner_DC.gpioB &&
		   expected_ESPinner_DC.mod == actual_ESPinner_DC.mod &&
		   expected_ESPinner_DC.ID == actual_ESPinner_DC.ID;
}

void loadStorage(const ESPinner_DC expectedList[]) {
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
			ESPinner_DC espinner;
			String output;
			serializeJson(obj, output);
			espinner.deserializeJSON(output);

			TEST_ASSERT_TRUE(
				compareESPinnerDC(expectedList[espinner_index], espinner));
			espinner_index++;
		}
	}
}

void checkESPinnerState(const String &id, int expectedDC_A, int expectedDC_B,
						ESPinner_Mod expectedType) {
	ESPinner *espinnerInList =
		ESPinner_Manager::getInstance().findESPinnerById(id);
	if (espinnerInList) {
		ESPinner_DC *DCEspinner = static_cast<ESPinner_DC *>(espinnerInList);
		if (DCEspinner) {
			TEST_ASSERT_EQUAL_INT8(expectedDC_A, DCEspinner->gpioA);
			TEST_ASSERT_EQUAL_INT8(expectedDC_B, DCEspinner->gpioB);
			TEST_ASSERT_EQUAL_STRING(id.c_str(), DCEspinner->getID().c_str());
			TEST_ASSERT_EQUAL(expectedType, DCEspinner->getType());
		} else {
			Serial.println("Error: ESPinner_DC Type Not Found");
		}
	} else {
		Serial.println("ESPinner ID Not Found In List");
	}
}

void test_void_DCespinner() {
	std::vector<ControlAssertion> void_DC_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "DC"},
		{firstESPinnerSelector, DC_PINA_SELECTOR_LABEL, "0"},
		{firstESPinnerSelector, DC_PINB_SELECTOR_LABEL, "0"},
		{firstESPinnerSelector, DC_SAVE_LABEL, DC_SAVE_VALUE},
		{firstESPinnerSelector, DC_REMOVE_LABEL, DC_REMOVE_VALUE}};

	assertControlValues(void_DC_assertions);

	// ESPAllOn_Manager Espinners Void
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(0, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());
}

void test_wrong_DCespinner() {
	std::vector<ControlAssertion> wrong_DC_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "DC"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_DC"},
		{firstESPinnerSelector, DC_PINA_SELECTOR_LABEL, "asdfg"},
		{firstESPinnerSelector, DC_PINB_SELECTOR_LABEL, "0"},
		{firstESPinnerSelector, DC_SAVE_LABEL, DC_SAVE_VALUE},
		{firstESPinnerSelector, DC_REMOVE_LABEL, DC_REMOVE_VALUE}};

	assertControlValues(wrong_DC_assertions);

	// ESPAllOn_Manager Espinners Void
	// Check EspAllOn_Pin_manager Status Not Connected
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(0, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());
}

void test_saved_DCespinner() {
	std::vector<ControlAssertion> saved_DC_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "DC"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_DC"},
		{firstESPinnerSelector, DC_PINA_SELECTOR_LABEL, "14"},
		{firstESPinnerSelector, DC_PINB_SELECTOR_LABEL, "15"},
		{firstESPinnerSelector, DC_SAVE_LABEL, DC_SAVE_VALUE},
		{firstESPinnerSelector, DC_REMOVE_LABEL, DC_REMOVE_VALUE}};

	assertControlValues(saved_DC_assertions);
	removedControl(firstESPinnerSelector, DC_PINA_SELECT_LABEL);
	removedControl(firstESPinnerSelector, DC_PINB_SELECT_LABEL);
	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());

	ESPinner_DC expected_espinner = ESPinner_DC();
	expected_espinner.setGPIOA(14);
	expected_espinner.setGPIOB(15);
	expected_espinner.setID("TEST_DC");
	ESPinner_DC expectedList[] = {expected_espinner};

	checkESPinnerState(expected_espinner.getID(), expected_espinner.getGPIOA(),
					   expected_espinner.getGPIOB(),
					   expected_espinner.getType());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //

	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeDC_AController->value.toInt()));
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeDC_BController->value.toInt()));

	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //
	loadStorage(expectedList);
}

void test_modified_DCespinner() {
	std::vector<ControlAssertion> modified_DC_assertions = {
		{firstESPinnerSelector, ESPINNERTYPE_LABEL, "DC"},
		{firstESPinnerSelector, ESPINNERID_LABEL, "TEST_DC"},
		{firstESPinnerSelector, DC_PINA_SELECTOR_LABEL, "16"},
		{firstESPinnerSelector, DC_PINB_SELECTOR_LABEL, "18"},
		{firstESPinnerSelector, DC_SAVE_LABEL, DC_SAVE_VALUE},
		{firstESPinnerSelector, DC_REMOVE_LABEL, DC_REMOVE_VALUE}};

	assertControlValues(modified_DC_assertions);
	removedControl(firstESPinnerSelector, DC_PINA_SELECT_LABEL);
	removedControl(firstESPinnerSelector, DC_PINB_SELECT_LABEL);
	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(2, controlReferences.size());

	ESPinner_DC expected_espinner = ESPinner_DC();
	expected_espinner.setGPIOA(16);
	expected_espinner.setGPIOB(18);
	expected_espinner.setID("TEST_DC");
	ESPinner_DC expectedList[] = {expected_espinner};

	checkESPinnerState(expected_espinner.getID(), expected_espinner.getGPIOA(),
					   expected_espinner.getGPIOB(),
					   expected_espinner.getType());

	DUMPLN("ESPINNER PIN A ", typeDC_AController->value.toInt());
	DUMPLN("ESPINNER PIN B ", typeDC_BController->value.toInt());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeDC_AController->value.toInt()));
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeDC_BController->value.toInt()));

	// Previous PIN are detached
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(14));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(15));

	// ----- Controllers Review ------ //
	// TO REVIEW
	TEST_ASSERT_EQUAL(
		3, ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	uint16_t parentRef = getParentId(elementToParentMap, firstESPinnerSelector);
	uint16_t controllerDIRRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, DC_SWITCH_DIR_LABEL);
	uint16_t controllerRUNRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, DC_SWITCH_RUN_LABEL);
	uint16_t controllerVELRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, DC_SLIDER_VEL_LABEL);

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

void test_second_void_DCespinner() {
	std::vector<ControlAssertion> void_DC_assertions = {
		{secondESPinnerSelector, ESPINNERTYPE_LABEL, "DC"},
		{secondESPinnerSelector, ESPINNERID_LABEL, "TEST_DC_2"},
		{secondESPinnerSelector, DC_PINA_SELECTOR_LABEL, "0"},
		{secondESPinnerSelector, DC_PINB_SELECTOR_LABEL, "0"},
		{secondESPinnerSelector, DC_PINA_SELECT_LABEL, DC_PINA_SELECT_VALUE},
		{secondESPinnerSelector, DC_PINB_SELECT_LABEL, DC_PINB_SELECT_VALUE},
		{secondESPinnerSelector, DC_SAVE_LABEL, DC_SAVE_VALUE},
		{secondESPinnerSelector, DC_REMOVE_LABEL, DC_REMOVE_VALUE}};

	assertControlValues(void_DC_assertions);

	// ESPAllOn_Manager Espinners Void
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(1, EspinnerListSize);
	TEST_ASSERT_EQUAL_INT16(3, controlReferences.size());

	TEST_ASSERT_EQUAL(
		3, ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());
}

void test_second_DCespinner() {
	std::vector<ControlAssertion> void_DC_assertions = {
		{secondESPinnerSelector, ESPINNERTYPE_LABEL, "DC"},
		{secondESPinnerSelector, ESPINNERID_LABEL, "TEST_DC_2"},
		{secondESPinnerSelector, DC_PINA_SELECTOR_LABEL, "20"},
		{secondESPinnerSelector, DC_PINB_SELECTOR_LABEL, "22"},
		{secondESPinnerSelector, DC_SAVE_LABEL, DC_SAVE_VALUE},
		{secondESPinnerSelector, DC_REMOVE_LABEL, DC_REMOVE_VALUE}};

	assertControlValues(void_DC_assertions);

	// -------------------------------------------- //
	// ------ Check Espinner_manager Status ------- //
	// -------------------------------------------- //
	uint8_t EspinnerListSize = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_INT8(2, EspinnerListSize);

	// REVIEW HERE ?=?= 2 ESPINNERS + 2 CONTROLLERS
	TEST_ASSERT_EQUAL_INT16(3, controlReferences.size());

	ESPinner_DC first_espinner = ESPinner_DC();
	first_espinner.setGPIOA(16);
	first_espinner.setGPIOB(18);
	first_espinner.setID("TEST_DC");

	ESPinner_DC expected_espinner = ESPinner_DC();
	expected_espinner.setGPIOA(20);
	expected_espinner.setGPIOB(22);
	expected_espinner.setID("TEST_DC_2");

	ESPinner_DC expectedList[] = {first_espinner, expected_espinner};

	checkESPinnerState(expected_espinner.getID(), expected_espinner.getGPIOA(),
					   expected_espinner.getGPIOB(),
					   expected_espinner.getType());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //

	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeDC_AController->value.toInt()));
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeDC_BController->value.toInt()));

	// ----- Controllers Review ------ //
	// TO REVIEW
	TEST_ASSERT_EQUAL(
		6, ESPinner_Manager::getInstance().getControllerMap().size());
	TEST_ASSERT_EQUAL(
		2, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	uint16_t parentRef =
		getParentId(elementToParentMap, secondESPinnerSelector);
	uint16_t controllerDIRRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, DC_SWITCH_DIR_LABEL);
	uint16_t controllerRUNRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, DC_SWITCH_RUN_LABEL);
	uint16_t controllerVELRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, DC_SLIDER_VEL_LABEL);

	TEST_ASSERT_EQUAL(parentRef,
					  ESPinner_Manager::getInstance().findRefByControllerId(
						  controllerDIRRef));
	TEST_ASSERT_EQUAL(parentRef,
					  ESPinner_Manager::getInstance().findRefByControllerId(
						  controllerRUNRef));
	TEST_ASSERT_EQUAL(parentRef,
					  ESPinner_Manager::getInstance().findRefByControllerId(
						  controllerVELRef));

	uint16_t firstParentRef =
		getParentId(elementToParentMap, firstESPinnerSelector);
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

	ESPinner_DC expected_espinner = ESPinner_DC();
	expected_espinner.setGPIOA(20);
	expected_espinner.setGPIOB(22);
	expected_espinner.setID("TEST_DC_2");
	checkESPinnerState(expected_espinner.getID(), expected_espinner.getGPIOA(),
					   expected_espinner.getGPIOB(),
					   expected_espinner.getType());

	// -------------------------------------------- //
	// ---- Check EspAllOn_Pin_manager Status ----- //
	// -------------------------------------------- //
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeDC_AController->value.toInt()));
	TEST_ASSERT_TRUE(ESPAllOnPinManager::getInstance().isPinAttached(
		typeDC_BController->value.toInt()));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(16));
	TEST_ASSERT_FALSE(ESPAllOnPinManager::getInstance().isPinAttached(18));

	TEST_ASSERT_EQUAL(
		3, ESPinner_Manager::getInstance().getControllerMap().size());

	TEST_ASSERT_EQUAL(
		1, ESPinner_Manager::getInstance().getUIRelationIDMap().size());

	// -------------------------------------------- //
	// ------ Check ESPinner Saved in memory ------ //
	// -------------------------------------------- //
	ESPinner_DC expectedList[] = {expected_espinner};
	loadStorage(expectedList);
}

void setup() {
	Serial.begin(115200);
	UNITY_BEGIN();

	RUN_TEST(test_no_elementsInParentMap);
	ESPAllOn::getInstance().setup(); // Executes ESPinnerSelector()

	RUN_TEST(test_firstSelectorOrderInelementParentMap);

	// ---------------------------------------------//
	// Define DC as INPUT and ID = TEST_DC
	// Search first ESPinnerSelector created
	// ---------------------------------------------//

	firstESPinnerSelector = elementToParentMap.begin()->second;
	uint16_t espinnerType_ref =
		searchByLabel(firstESPinnerSelector, ESPINNERTYPE_LABEL);
	Control *typeController = ESPUI.getControl(espinnerType_ref);
	typeController->value = DC_LABEL; // This is important to be DC

	uint16_t DCID_ref = searchByLabel(firstESPinnerSelector, ESPINNERID_LABEL);
	Control *typeDCIDController = ESPUI.getControl(DCID_ref);
	typeDCIDController->value = "TEST_DC";

	// Click on Save Generic ESPinner and creates a new one
	saveElement_callback(typeController, B_UP);

	RUN_TEST(test_void_DCespinner);

	// ---------------------------------------------//
	// DC_MODESELECTOR_LABEL
	// REVIEW DC STRUCTURE
	// ---------------------------------------------//
	uint16_t DCMode_ref =
		searchByLabel(firstESPinnerSelector, DC_MODESELECTOR_LABEL);

	// Input Wrong Number
	uint16_t DCA_ref =
		searchByLabel(firstESPinnerSelector, DC_PINA_SELECTOR_LABEL);
	typeDC_AController = ESPUI.getControl(DCA_ref);
	typeDC_AController->value = "asdfg";

	// Click On save, but not saved because of wrong number in DC PIN
	DCSelector_callback(typeDC_AController, B_UP);
	saveButtonGPIOCheck(firstESPinnerSelector, DC_PINA_SELECTOR_LABEL,
						DC_action);
	RUN_TEST(test_wrong_DCespinner);

	// ---------------------------------------------//
	// Select a correct Number
	// Click On Save DC
	// ---------------------------------------------//
	typeDC_AController->value = "14";

	uint16_t DCB_ref =
		searchByLabel(firstESPinnerSelector, DC_PINB_SELECTOR_LABEL);
	typeDC_BController = ESPUI.getControl(DCB_ref);
	typeDC_BController->value = "15";

	DCSelector_callback(typeDC_AController, B_UP);
	DCSelector_callback(typeDC_BController, B_UP);
	saveButtonGPIOCheck(firstESPinnerSelector, DC_PINA_SELECTOR_LABEL,
						DC_action);
	saveButtonGPIOCheck(firstESPinnerSelector, DC_PINB_SELECTOR_LABEL,
						DC_action);
	RUN_TEST(test_saved_DCespinner);

	// ---------------------------------------------//
	// CHANGE PIN NUMBER TO OTHER DIFFERENT
	// Check Attached and detached PINs
	// ---------------------------------------------//
	typeDC_AController->value = "16";
	saveDC_callback(typeDC_AController, B_UP);
	typeDC_BController->value = "18";
	saveDC_callback(typeDC_AController, B_UP);
	RUN_TEST(test_modified_DCespinner);

	// -------------------------------------------- //
	// ----------- Second GUI ESPinner ------------ //
	// -------------------------------------------- //

	std::set<uint16_t> ListRefs = collectParentRefs(elementToParentMap);
	currentParentRefs = filterParentRef(ListRefs, firstESPinnerSelector);

	secondESPinnerSelector = *currentParentRefs.begin();

	// Create a new DC
	uint16_t second_espinnerType_ref =
		searchByLabel(secondESPinnerSelector, ESPINNERTYPE_LABEL);
	Control *second_typeController = ESPUI.getControl(second_espinnerType_ref);
	second_typeController->value = DC_LABEL; // This is important to be DC

	uint16_t second_DCID_ref =
		searchByLabel(secondESPinnerSelector, ESPINNERID_LABEL);
	Control *second_typeDCIDController = ESPUI.getControl(second_DCID_ref);
	second_typeDCIDController->value = "TEST_DC_2";

	// Click on Save Generic ESPinner and creates a new one
	saveElement_callback(second_typeController, B_UP);

	RUN_TEST(test_second_void_DCespinner);

	// -------------------------------------------- //
	// ----------- Save Second GUI ESPinner ------------ //
	// -------------------------------------------- //

	uint16_t second_DC_ref =
		searchByLabel(secondESPinnerSelector, DC_PINA_SELECTOR_LABEL);
	typeDC_AController = ESPUI.getControl(second_DC_ref);
	typeDC_AController->value = "20";

	uint16_t second_DCB_ref =
		searchByLabel(secondESPinnerSelector, DC_PINB_SELECTOR_LABEL);
	typeDC_BController = ESPUI.getControl(second_DCB_ref);
	typeDC_BController->value = "22";

	DCSelector_callback(typeDC_AController, B_UP);
	DCSelector_callback(typeDC_BController, B_UP);
	saveButtonGPIOCheck(secondESPinnerSelector, DC_PINA_SELECTOR_LABEL,
						DC_action);
	saveButtonGPIOCheck(secondESPinnerSelector, DC_PINB_SELECTOR_LABEL,
						DC_action);
	saveDC_callback(second_typeController, B_UP);
	RUN_TEST(test_second_DCespinner);

	// Define DC as OUTPUT and ID = TEST_OUTPUT
	// Input the last DC Number which is occupied
	// This should not be referenced again

	// Change input DC
	// Click On Save DC

	// -------------------------------------------- //
	// ----------- Remove First DC ------------ //
	// -------------------------------------------- //

	uint16_t espinnerRemove_ref =
		searchByLabel(firstESPinnerSelector, DC_REMOVE_LABEL);
	Control *second_removeController = ESPUI.getControl(espinnerRemove_ref);
	removeDC_callback(second_removeController, B_UP);

	RUN_TEST(test_firstESPinner_removed);

	UNITY_END();
}

void loop() {}