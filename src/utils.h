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
#define INFO_COLOR "#22ccb3"

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
template <typename MapType>
uint16_t GUI_TextField(uint16_t parentRef, const char *GPIOLabel,
					   const char *GPIOValue, UICallback SelectorCallback,
					   MapType &targetMap);

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

/** Map to store WiFi UI control references by label */
std::map<String, uint16_t> WIFI_UI_ref;

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

// ------------------------------------- //
// --------- GENERIC MAP UTILITIES ----- //
// ------------------------------------- //

/**
 * Generic method to add element-parent relationship to any map
 * @param map Reference to the map to modify
 * @param elementId Element ID to add
 * @param parentId Parent ID to associate with element
 */
template <typename K, typename V>
void addRelation(std::map<K, V> &map, K elementId, V parentId) {
	map[elementId] = parentId;
}

/**
 * Generic method to remove element by key from any map
 * @param map Reference to the map to modify
 * @param key Key to remove
 */
template <typename K, typename V> void removeByKey(std::map<K, V> &map, K key) {
	map.erase(key);
}

/**
 * Generic method to remove all elements with specific value from map
 * @param map Reference to the map to modify
 * @param value Value to match for removal
 */
template <typename K, typename V>
void removeByValue(std::map<K, V> &map, V value) {
	std::vector<K> keysToRemove;
	for (const auto &pair : map) {
		if (pair.second == value) {
			keysToRemove.push_back(pair.first);
		}
	}
	for (K key : keysToRemove) {
		map.erase(key);
	}
}

/**
 * Generic method to find value by key in map
 * @param map Reference to the map to search
 * @param key Key to search for
 * @param defaultValue Default value if key not found
 * @return Value if found, defaultValue otherwise
 */
template <typename K, typename V>
V findByKey(const std::map<K, V> &map, K key, V defaultValue = V{}) {
	auto it = map.find(key);
	return (it != map.end()) ? it->second : defaultValue;
}

/**
 * Generic method to debug any map
 * @param map Reference to the map to debug
 * @param mapName Name of the map for debugging output
 */
template <typename K, typename V>
void debugGenericMap(const std::map<K, V> &map, const String &mapName) {
	DUMP("Map Name: ", mapName);
	DUMPLN(" size: ", map.size());
	for (const auto &pair : map) {
		DUMP("  Key: ", pair.first);
		DUMPLN(" -> Value: ", pair.second);
	}
}

/**
 * Adds an element with its parent to a map (legacy function - use addRelation
 * instead)
 */
template <typename K, typename V>
void addElementWithParent(std::map<K, V> &map, K elementId, V parentId) {
	addRelation(map, elementId, parentId);
}

/**
 * Debug function to print map contents (legacy function - use debugGenericMap
 * instead)
 * @param map Map to debug
 */
template <typename K, typename V> void debugMap(const std::map<K, V> &map) {
	debugGenericMap(map, "Map");
}

/**
 * Gets parent ID from a generic map (legacy function - use findByKey instead)
 * @param map Map to search in
 * @param elementId Element ID to find parent for
 * @return Parent ID if found, 0 otherwise
 */
template <typename K, typename V>
V getParentId(const std::map<K, V> &map, K elementId) {
	V result = findByKey(map, elementId, V{});
	if (result == V{}) {
		DUMPLN("THERE IS NO PARENT FROM HERE: ", elementId);
	}
	return result;
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

// Remove Key from generic Map (legacy function - use removeByKey instead)
template <typename K, typename V>
void removeValueFromMap(std::map<K, V> &elementToParentMap, K key) {
	removeByKey(elementToParentMap, key);
}

// Remove Children from generic Map (legacy function - use removeByValue
// instead)
template <typename K, typename V>
void removeChildrenFromMap(std::map<K, V> &elementToParentMap, V value) {
	removeByValue(elementToParentMap, value);
}

// Remove Key from generic Map (legacy function - use removeByKey instead)
template <typename K, typename V>
void removeKeyFromMap(std::map<K, V> &elementToParentMap, K key) {
	removeByKey(elementToParentMap, key);
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
	DUMPLN("MEMORY FAIL: LABEL NOT FOUND: ", label);
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
	DUMPLN("MEMORY FAIL: VALUE NOT FOUND: ", value);
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

bool saveButtonGPIOCheck(uint16_t parentRef, const char *label,
						 void (*GPIO_Actions)(uint16_t),
						 const char *saveLabel) {

	uint16_t GPIOSelectorRef = searchByLabel(parentRef, label);

	String GPIOSelector_value;
	if (GPIOSelectorRef != 0) {
		GPIOSelector_value = ESPUI.getControl(GPIOSelectorRef)->value;
		// TODO if GPIOSelector_value exists -> Check if ESP_PinMode List
		// related with Container exists. Create new One
	}

	uint16_t SaveButtonRef = searchByLabel(parentRef, saveLabel);

	if (isNumericString(GPIOSelector_value)) {
		char *backgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(SaveButtonRef, backgroundStyle);

		// Execute Action when save from each mod
		GPIO_Actions(parentRef);
	} else {
		char *backgroundStyle = getBackground(DANGER_COLOR);
		ESPUI.setElementStyle(SaveButtonRef, backgroundStyle);
	}
	return true;
}

#endif