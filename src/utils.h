#include <Arduino.h>
#include <ESPUI.h>

// ------------------ SERIAL CONFIG --------------------//
#define PRINTDEBUG true
#define SERIALDEBUG Serial

#define DUMPS(s)                                                               \
	{ SERIALDEBUG.print(F(s)); }
#define DUMPSLN(s)                                                             \
	{ SERIALDEBUG.println(F(s)); }
#define DUMPPRINTLN()                                                          \
	{ SERIALDEBUG.println(); }
#define DUMPV(v)                                                               \
	{ SERIALDEBUG.print(v); }
#define DUMPLN(s, v)                                                           \
	{                                                                          \
		DUMPS(s);                                                              \
		DUMPV(v);                                                              \
		DUMPPRINTLN();                                                         \
	}
#define DUMP(s, v)                                                             \
	{                                                                          \
		DUMPS(s);                                                              \
		DUMPV(v);                                                              \
	}

#define DANGER_COLOR "#f55d76"
#define SUCCESS_COLOR "#8ced9b"

using UICallback = void (*)(Control *sender, int type);

void removeElement_callback(Control *sender, int type);
void saveElement_callback(Control *sender, int type);
void GUI_GPIOSelector(uint16_t parentRef, const char *GPIOLabel,
					  const char *GPIOValue, UICallback SelectorCallback);
/*----------------------------------------------------*/
/*----------------   Vector List  --------------------*/
/*----------------------------------------------------*/

// Array List of Containers inside mainSelector in Mod Tabs
std::vector<uint16_t> controlReferences;

// Map list to link children elements to parent container < child , parent >
/**
 *            o ( child UID )
 *           /
 *  Parent  O
 *           \
 *            o ( child GPIO )
 */

std::map<uint16_t, uint16_t> elementToParentMap;

void addElementWithParent(std::map<uint16_t, uint16_t> &map, uint16_t elementId,
						  uint16_t parentId) {
	map[elementId] = parentId;
}

void debugMap(std::map<uint16_t, uint16_t> &map) {
	for (auto it = map.begin(); it != map.end(); ++it) {
		DUMP(" Key: ", it->first);
		DUMP(" Value: ", it->second);
	}
}

uint16_t getParentId(const std::map<uint16_t, uint16_t> &map,
					 uint16_t elementId) {
	auto it = map.find(elementId); // "it" refers to the element found
	if (it != map.end()) {
		return it->second;
	} else {
		return 0;
	}
}

std::vector<uint16_t> getChildrenIds(const std::map<uint16_t, uint16_t> &map,
									 uint16_t parentId) {
	std::vector<uint16_t> childrenIds;
	for (const auto &pair : map) {
		if (pair.second == parentId) {
			childrenIds.push_back(pair.first);
		}
	}
	return childrenIds;
}

void removeControlId(std::vector<uint16_t> &controlRef, uint16_t id) {
	auto newEnd = std::remove(controlRef.begin(), controlRef.end(), id);
	controlRef.erase(newEnd, controlRef.end());
}

void removeValueFromMap(std::map<uint16_t, uint16_t> &elementToParentMap,
						uint16_t value) {
	std::vector<uint16_t> keysToRemove;
	int numErased = elementToParentMap.erase(value);
	// if (numErased == 0) {
	// 	DUMPLN("No Element in map : ", numErased);
	// } else {
	// 	DUMPLN("REMOVED ID : ", numErased);
	// }
}

void removeChildrenFromMap(std::map<uint16_t, uint16_t> &elementToParentMap,
						   uint16_t value) {
	std::vector<uint16_t> keysToRemove;
	for (const auto &pair : elementToParentMap) {
		if (pair.second == value) {
			keysToRemove.push_back(pair.first);
		}
	}

	for (uint16_t key : keysToRemove) {
		elementToParentMap.erase(key);
	}
}

uint16_t searchByValue(uint16_t element_id, String value) {
	std::vector<uint16_t> childrenIds =
		getChildrenIds(elementToParentMap, element_id);
	for (uint16_t childControllerId : childrenIds) {
		String childController_value =
			String(ESPUI.getControl(childControllerId)->value);
		if (childController_value == value) {
			return childControllerId;
		}
	}
	return 0;
}

uint16_t searchByLabel(uint16_t element_id, String label) {
	std::vector<uint16_t> childrenIds =
		getChildrenIds(elementToParentMap, element_id);
	for (uint16_t childControllerId : childrenIds) {
		String childController_label =
			String(ESPUI.getControl(childControllerId)->label);
		if (childController_label == label) {
			return childControllerId;
		}
	}
	return 0;
}

char *getBackground(const char *color) {
	static char buffer[40];
	snprintf(buffer, sizeof(buffer), "background-color: %s;", color);
	return buffer;
}
#define PINSIZE 40
// #define PINSIZE ESP_BoardConf::NUM_PINS

bool isNumericString(String str) {
	return (str.toInt() != 0 && str.toInt() < PINSIZE);
}

enum class labelPin {
	PIN0 = 0,
	PIN1,
	PIN2,
	PIN3,
	PIN4,
	PIN5,
	PIN6,
	PIN7,
	PIN8,
	PIN9,
	PIN10,
	PIN11,
	PIN12,
	PIN13,
	PIN14,
	PIN15,
	PIN16,
	PIN17,
	PIN18,
	PIN19,
	PIN20,
	PIN21,
	PIN22,
	PIN23,
	PIN24,
	PIN25,
	PIN26,
	PIN27,
	PIN28,
	PIN29,
	PIN30,
	PIN31,
	PIN32,
	PIN33,
	PIN34,
	PIN35,
	PIN36,
	PIN37,
	PIN38,
	PIN39,
	PIN40,
};

// Array of pinLabels to associate with gpioLabels

const std::array<std::string, static_cast<size_t>(PINSIZE)> pinLabels = {
	"1",  "2",	"3",  "4",	"5",  "6",	"7",  "8",	"9",  "10",
	"11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
	"21", "22", "23", "24", "25", "26", "27", "28", "29", "30",
	"31", "32", "33", "34", "35", "36", "37", "38", "39", "40"};

std::string getPinLabel(labelPin pin) {
	return pinLabels[static_cast<size_t>(pin)];
}

// Available List of GPIOs currently

std::map<uint8_t, const char *> gpioLabels = {
	{0, "0"},	{1, "1"},	{2, "2"},	{3, "3"},	{4, "4"},	{5, "5"},
	{6, "6"},	{7, "7"},	{8, "8"},	{9, "9"},	{10, "10"}, {11, "11"},
	{12, "12"}, {13, "13"}, {14, "14"}, {15, "15"}, {16, "16"}, {17, "17"},
	{18, "18"}, {19, "19"}, {20, "20"}, {21, "21"}, {22, "22"}, {23, "23"},
	{24, "24"}, {25, "25"}, {26, "26"}, {27, "27"}, {28, "28"}, {29, "29"},
	{30, "30"}, {31, "31"}, {32, "32"}, {33, "33"}, {34, "34"}, {35, "35"},
	{36, "36"}, {37, "37"}, {38, "38"}, {39, "39"}};

const char *getLabelForPin(uint8_t pin) {
	if (gpioLabels.find(pin) != gpioLabels.end()) {
		return gpioLabels[pin];
	} else {
		return "Pin not found";
	}
}

const char *getLabelOnList(std::map<uint8_t, const char *> &list, uint8_t pin) {
	if (list.find(pin) != list.end()) {
		return list[pin];
	} else {
		return "Pin not found";
	}
}

void addLabelOnList(std::map<uint8_t, const char *> &list,
					std::pair<uint8_t, const char *> labelPair) {
	list[labelPair.first] = labelPair.second;
}

void removeLabelOnList(std::map<uint8_t, const char *> &list,
					   std::pair<uint8_t, const char *> labelPair) {
	list.erase(labelPair.first);
}

void removeLabelOnList(std::map<uint8_t, const char *> &list, uint8_t gpio) {
	auto it = list.find(gpio);
	if (it != list.end()) {
		list.erase(it);
	}
}

void addGPIOLabel(std::pair<uint8_t, const char *> gpioLabelPair) {
	addLabelOnList(gpioLabels, gpioLabelPair);
}

void removeGPIOLabel(std::pair<uint8_t, const char *> gpioLabelPair) {
	removeLabelOnList(gpioLabels, gpioLabelPair);
}

void removeGPIOLabel(uint8_t gpio) { removeLabelOnList(gpioLabels, gpio); }

//-------------- UI --------------------//

void GUI_SaveButton(uint16_t parentRef, const char *saveLabel,
					const char *saveValue, UICallback saveCallback) {
	// Save Element Button
	uint16_t GPIOAdd_selector =
		ESPUI.addControl(ControlType::Button, saveLabel, saveValue,
						 ControlColor::Alizarin, parentRef, saveCallback);
	addElementWithParent(elementToParentMap, GPIOAdd_selector,
						 parentRef); // Add Save Button to parent
}
void GUI_RemoveButton(uint16_t parentRef, const char *removeLabel,
					  const char *removeValue, UICallback removeCallback) {
	// Remove Element Button
	uint16_t GPIORemove_selector =
		ESPUI.addControl(ControlType::Button, removeLabel, removeValue,
						 ControlColor::Alizarin, parentRef, removeCallback);

	addElementWithParent(elementToParentMap, GPIORemove_selector,
						 parentRef); // Add Remove Button to parent
}

void GUIButtons_Elements(uint16_t parentRef, const char *saveLabel,
						 const char *saveValue, const char *removeLabel,
						 const char *removeValue, UICallback saveCallback,
						 UICallback removeCallback) {
	GUI_SaveButton(parentRef, saveLabel, saveValue, saveCallback);
	GUI_RemoveButton(parentRef, removeLabel, removeValue, removeCallback);
	// debugMap(elementToParentMap);
}

void GUI_GPIOSetLabel(uint16_t parentRef, const char *GPIOLabel,
					  const char *GPIOValue) {
	uint16_t GPIOLabel_selector =
		ESPUI.addControl(ControlType::Label, GPIOLabel, GPIOValue,
						 ControlColor::Carrot, parentRef);
	char *backgroundStyle = getBackground(DANGER_COLOR);
	ESPUI.setElementStyle(GPIOLabel_selector, backgroundStyle);
	addElementWithParent(elementToParentMap, GPIOLabel_selector,
						 parentRef); // Add GPIO Label to parent
}

void saveButtonCheck(uint16_t parentRef, const char *SelectorLabel,
					 const char *SaveButtonLabel) {
	uint16_t GPIOSelectorRef = searchByLabel(parentRef, SelectorLabel);
	uint16_t SaveButtonRef = searchByLabel(parentRef, SaveButtonLabel);
	String GPIOSelector_value = ESPUI.getControl(GPIOSelectorRef)->value;

	if (GPIOSelector_value == "0" || isNumericString(GPIOSelector_value)) {
		char *backgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(SaveButtonRef, backgroundStyle);
		// Save ESPINNER

	} else {
		char *backgroundStyle = getBackground(DANGER_COLOR);
		ESPUI.setElementStyle(SaveButtonRef, backgroundStyle);
	}
}
