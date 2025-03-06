#ifndef _ESPALLON_UTILS_H
#define _ESPALLON_UTILS_H
#include <Arduino.h>
#include <ESPUI.h>

#include "models/ESP_Boards.h"
// ------------------ SERIAL CONFIG --------------------//
#define PRINTDEBUG true
#define SERIALDEBUG Serial

#define DUMPS(s)                                                               \
	{                                                                          \
		SERIALDEBUG.print(F(s));                                               \
	}
#define DUMPSLN(s)                                                             \
	{                                                                          \
		SERIALDEBUG.println(F(s));                                             \
	}
#define DUMPPRINTLN()                                                          \
	{                                                                          \
		SERIALDEBUG.println();                                                 \
	}
#define DUMPV(v)                                                               \
	{                                                                          \
		SERIALDEBUG.print(v);                                                  \
	}
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

#define DANGER_COLOR "#cc3333"
#define SUCCESS_COLOR "#33cc66"
#define PENDING_COLOR "#6633dd"
#define SELECTED_COLOR "#1165aa"

using UICallback = void (*)(Control *sender, int type);

void removeElement_callback(Control *sender, int type);
void saveElement_callback(Control *sender, int type);
uint16_t GUI_GPIOSelector(uint16_t parentRef, const char *GPIOLabel,
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
	DUMPLN("SIZE MAP: ", map.size());
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
		DUMPLN("THERE IS NO PARENT FROM HERE: ", elementId);
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
	DUMPSLN("MEMORY FAIL: VALUE  NOT FOUND");
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
	DUMPSLN("MEMORY FAIL: LABEL NOT FOUND");
	return 0;
}

char *getBackground(const char *color) {
	static char buffer[40];
	snprintf(buffer, sizeof(buffer), "background-color: %s;", color);
	return buffer;
}

#define PINSIZE ESP_BoardConf::NUM_PINS

bool isNumericString(String str) {
	return (str.toInt() != 0 && str.toInt() < PINSIZE);
}

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

	if (GPIOSelectorRef == 0) {
		return;
	}
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
#endif