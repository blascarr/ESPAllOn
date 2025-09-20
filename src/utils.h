#ifndef _ESPALLON_UTILS_H
#define _ESPALLON_UTILS_H
#include <Arduino.h>
#include <ESPUI.h>

#include "models/ESP_Boards.h"
// ------------------ SERIAL CONFIG --------------------//
#define PRINTDEBUG true

#if PRINTDEBUG

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

#else
#define DUMPS(s)                                                               \
	{                                                                          \
	}
#define DUMPSLN(s)                                                             \
	{                                                                          \
	}
#define DUMPPRINTLN()                                                          \
	{                                                                          \
	}
#define DUMPV(v)                                                               \
	{                                                                          \
	}
#define DUMPLN(s, v)                                                           \
	{                                                                          \
	}
#define DUMP(s, v)                                                             \
	{                                                                          \
	}

#endif

/** Color scheme definitions for UI elements */
#define DANGER_COLOR "#cc3333"
#define SUCCESS_COLOR "#33cc66"
#define PENDING_COLOR "#6633dd"
#define SELECTED_COLOR "#1165aa"

/** Type alias for UI callback functions */
using UICallback = void (*)(Control *sender, int type);

/**
 * Callback function for removing UI elements
 */
void removeElement_callback(Control *sender, int type);

/**
 * Callback function for saving UI elements
 */
void saveElement_callback(Control *sender, int type);

/**
 * Creates a GPIO selector GUI element
 * @param parentRef Reference to the parent UI control
 * @param GPIOLabel Label for the GPIO selector
 * @param GPIOValue Default value for the GPIO selector
 * @param SelectorCallback Callback function for the selector
 * @return Control ID of the created GPIO selector
 */
uint16_t GUI_GPIOSelector(uint16_t parentRef, const char *GPIOLabel,
						  const char *GPIOValue, UICallback SelectorCallback);

/**
 * Detaches a removed PIN from the system
 * @param expected_label Expected label for validation
 * @param espinner_label ESPinner label
 * @param espinner_value ESPinner value containing pin number
 */
void detachRemovedPIN(String expected_label, String espinner_label,
					  String espinner_value);
/*----------------------------------------------------*/
/*----------------   Vector List  --------------------*/
/*----------------------------------------------------*/

/** Array list of container references inside main selector in module tabs */
std::vector<uint16_t> controlReferences;

/**
 * Map to link children elements to parent container <child, parent>
 *
 * Structure:
 *            o ( child UID )
 *           /
 *  Parent  O
 *           \
 *            o ( child GPIO )
 */
std::map<uint16_t, uint16_t> elementToParentMap;

/**
 * Gets all child IDs for a given parent ID from the map
 * @param map Map containing child-parent relationships
 * @param parentId ID of the parent to search for
 * @return Vector containing all child IDs
 */
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

/**
 * Adds an element with its parent to a map
 */
template <typename K, typename V>
void addElementWithParent(std::map<K, V> &map, K elementId, V parentId) {
	map[elementId] = parentId;
}

/**
 * Debug function to print map contents
 * @param map Map to debug
 */
template <typename K, typename V> void debugMap(const std::map<K, V> &map) {
	DUMPLN("SIZE MAP: ", map.size());
	for (const auto &it : map) {
		DUMP(" Key: ", it.first);
		DUMP(" Value: ", it.second);
	}
}

/**
 * Gets parent ID from a generic map where K is the key and V is the
 * value
 * @param map Map to search in
 * @param elementId Element ID to find parent for
 * @return Parent ID if found, 0 otherwise
 */
template <typename K, typename V>
V getParentId(const std::map<K, V> &map, K elementId) {
	auto it = map.find(elementId); // "it" refers to the element found
	if (it != map.end()) {
		return it->second;
	} else {
		DUMPLN("THERE IS NO PARENT FROM HERE: ", elementId);
		return 0;
	}
}

/**
 * Removes control ID from vector of control references
 * @param controlRef Vector of control references
 * @param id ID to remove from the vector
 */
void removeControlId(std::vector<uint16_t> &controlRef, uint16_t id) {
	auto newEnd = std::remove(controlRef.begin(), controlRef.end(), id);
	controlRef.erase(newEnd, controlRef.end());
}

// Remove Key from generic Map template <K, V> where K is the key and V is the
// value. ElementId is the key and remove the key from the map.
template <typename K, typename V>
void removeValueFromMap(std::map<K, V> &elementToParentMap, K key) {
	int numErased = elementToParentMap.erase(key);
	// if (numErased == 0) {
	// 	DUMPLN("No Element in map : ", numErased);
	// } else {
	// 	DUMPLN("REMOVED ID : ", numErased);
	// }
}

// Remove Children from generic Map template <K, V> .
// ElementId is the key and remove the children from the map.
template <typename K, typename V>
void removeChildrenFromMap(std::map<K, V> &elementToParentMap, V value) {
	std::vector<K> keysToRemove;
	for (const auto &pair : elementToParentMap) {
		if (pair.second == value) {
			keysToRemove.push_back(pair.first);
		}
	}

	for (K key : keysToRemove) {
		elementToParentMap.erase(key);
	}
}

// Remove Key from generic Map template <K, V>.
// ElementId is the key and remove the key from the map.
template <typename K, typename V>
void removeKeyFromMap(std::map<K, V> &elementToParentMap, K key) {
	elementToParentMap.erase(key);
}

template <typename K, typename V>
uint16_t searchInMapByLabel(const std::map<K, V> &relationMap,
							uint16_t element_id, String label) {

	std::vector<uint16_t> childrenIds = getChildrenIds(relationMap, element_id);

	for (uint16_t childControllerId : childrenIds) {
		String childController_label =
			String(ESPUI.getControl(childControllerId)->label);
		if (childController_label == label) {
			return childControllerId; // Retornar ID si encuentra coincidencia
		}
	}

	DUMPSLN("MEMORY FAIL: LABEL NOT FOUND");
	return 0;
}

template <typename K, typename V>
uint16_t searchInMapByValue(const std::map<K, V> &relationMap,
							uint16_t element_id, String value) {
	// Obtener los hijos del elemento en el mapa especificado
	std::vector<uint16_t> childrenIds = getChildrenIds(relationMap, element_id);

	// Buscar en la lista de hijos comparando el `value`
	for (uint16_t childControllerId : childrenIds) {
		String childController_value =
			String(ESPUI.getControl(childControllerId)->value);
		if (childController_value == value) {
			return childControllerId; // Retornar ID si encuentra coincidencia
		}
	}

	// Mensaje de error si no se encuentra el valor
	DUMPSLN("MEMORY FAIL: VALUE NOT FOUND");
	return 0;
}

uint16_t searchByValue(uint16_t element_id, String value) {
	return searchInMapByValue(elementToParentMap, element_id, value);
}

uint16_t searchByLabel(uint16_t element_id, String label) {
	return searchInMapByLabel(elementToParentMap, element_id, label);
}

/**
 * Generates CSS background color style string
 * @param color Color code in hexadecimal format (e.g., "#ff0000")
 * @return CSS style string for background color
 */
char *getBackground(const char *color) {
	static char buffer[40];
	snprintf(buffer, sizeof(buffer), "background-color: %s;", color);
	return buffer;
}

#define PINSIZE ESP_BoardConf::NUM_PINS

/**
 * Checks if a string represents a valid numeric pin value
 * @param str String to check
 * @return True if string is numeric and within valid pin range
 */
bool isNumericString(String str) {
	return (str.toInt() != 0 && str.toInt() < PINSIZE);
}

/**
 * @brief Validates if a string contains only numeric characters
 * @param value The string to validate
 * @return true if the string contains only digits, false otherwise
 */
bool isValidNumericString(const String &value) {
	if (value.length() == 0) {
		return false;
	}

	for (size_t i = 0; i < value.length(); i++) {
		if (!isdigit(value.charAt(i))) {
			return false;
		}
	}

	return true;
}

//-------------- UI --------------------//

/**
 * Creates a save button for GUI elements
 * @param parentRef Reference to the parent UI control
 * @param saveLabel Label for the save button
 * @param saveValue Value for the save button
 * @param saveCallback Callback function for save action
 */
void GUI_SaveButton(uint16_t parentRef, const char *saveLabel,
					const char *saveValue, UICallback saveCallback) {
	// Save Element Button
	uint16_t GPIOAdd_selector =
		ESPUI.addControl(ControlType::Button, saveLabel, saveValue,
						 ControlColor::Alizarin, parentRef, saveCallback);
	addElementWithParent(elementToParentMap, GPIOAdd_selector,
						 parentRef); // Add Save Button to parent
}
/**
 * Creates a remove button for GUI elements
 * @param parentRef Reference to the parent UI control
 * @param removeLabel Label for the remove button
 * @param removeValue Value for the remove button
 * @param removeCallback Callback function for remove action
 */
void GUI_RemoveButton(uint16_t parentRef, const char *removeLabel,
					  const char *removeValue, UICallback removeCallback) {
	// Remove Element Button
	uint16_t GPIORemove_selector =
		ESPUI.addControl(ControlType::Button, removeLabel, removeValue,
						 ControlColor::Alizarin, parentRef, removeCallback);

	addElementWithParent(elementToParentMap, GPIORemove_selector,
						 parentRef); // Add Remove Button to parent
}

/**
 * Creates both save and remove buttons for GUI elements
 * @param parentRef Reference to the parent UI control
 * @param saveLabel Label for the save button
 * @param saveValue Value for the save button
 * @param removeLabel Label for the remove button
 * @param removeValue Value for the remove button
 * @param saveCallback Callback function for save action
 * @param removeCallback Callback function for remove action
 */
void GUIButtons_Elements(uint16_t parentRef, const char *saveLabel,
						 const char *saveValue, const char *removeLabel,
						 const char *removeValue, UICallback saveCallback,
						 UICallback removeCallback) {
	GUI_SaveButton(parentRef, saveLabel, saveValue, saveCallback);
	GUI_RemoveButton(parentRef, removeLabel, removeValue, removeCallback);
}

void GUI_setLabel(uint16_t parentRef, const char *label, const char *value,
				  const char *color = DANGER_COLOR) {
	uint16_t Label_selector = ESPUI.addControl(ControlType::Label, label, value,
											   ControlColor::Carrot, parentRef);
	char *backgroundStyle = getBackground(color);
	ESPUI.setElementStyle(Label_selector, backgroundStyle);
	addElementWithParent(elementToParentMap, Label_selector,
						 parentRef); // Add Label to parent
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

void saveButtonGPIOCheck(uint16_t parentRef, const char *label,
						 void (*GPIO_Actions)(uint16_t)) {

	uint16_t GPIOSelectorRef = searchByLabel(parentRef, label);

	String GPIOSelector_value;
	if (GPIOSelectorRef != 0) {
		GPIOSelector_value = ESPUI.getControl(GPIOSelectorRef)->value;
		// TODO if GPIOSelector_value exists -> Check if ESP_PinMode List
		// related with Container exists. Create new One
	}

	uint16_t SaveButtonRef = searchByLabel(parentRef, GPIO_SAVE_LABEL);

	if (isNumericString(GPIOSelector_value)) {
		char *backgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(SaveButtonRef, backgroundStyle);

		// Execute Action when save from each mod
		GPIO_Actions(parentRef);
	} else {
		char *backgroundStyle = getBackground(DANGER_COLOR);
		ESPUI.setElementStyle(SaveButtonRef, backgroundStyle);
	}
}

#endif