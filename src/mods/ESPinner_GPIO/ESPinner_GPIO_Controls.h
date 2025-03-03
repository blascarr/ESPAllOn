#ifndef _ESPINNER_GPIO_CONTROLS_H
#define _ESPINNER_GPIO_CONTROLS_H

#include "./ESPinner_GPIO.h"

#include "../../manager/ESPinner_Manager.h"
/*
 *	GPIO_ModeSelector
 *	Select_GPIO
 *	GPIO_PinSelector
 *	GPIOSave
 *	GPIORemove
 */
void createGPIOMod_callback(Control *sender, int type) {
	DUMPSLN("CREATE GPIO PIN");
}

void saveButtonGPIOCheck(uint16_t parentRef) {

	uint16_t GPIOSelectorRef = searchByLabel(parentRef, GPIO_PINSELECTOR_LABEL);

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

		// Save ESPINNER
		auto espinnerGPIO = std::make_unique<ESPinner_GPIO>();
		std::vector<uint16_t> childrenIds =
			getChildrenIds(elementToParentMap, parentRef);
		for (uint16_t childControllerId : childrenIds) {
			String espinner_value =
				String(ESPUI.getControl(childControllerId)->value);
			String espinner_label =
				String(ESPUI.getControl(childControllerId)->label);
			DUMPLN("Label ESPinner: ", espinner_label);
			DUMPLN("Value ESPinner: ", espinner_value);
			if (espinner_label == GPIO_MODESELECTOR_LABEL) {
				String espinnerMode_value =
					String(ESPUI.getControl(childControllerId)->value);
				if (espinnerMode_value == GPIO_ESPINNERINPUT_LABEL) {
					espinnerGPIO->setGPIOMode(GPIOMode::Input);
				} else if (espinnerMode_value == GPIO_ESPINNEROUTPUT_LABEL) {
					espinnerGPIO->setGPIOMode(GPIOMode::Output);
				}
			}

			if (espinner_label == ESPINNERID_LABEL) {
				espinnerGPIO->setID(espinner_value);
			}

			if (espinner_label == GPIO_PINSELECTOR_LABEL) {
				espinnerGPIO->setGPIO(espinner_value.toInt());
			}
		}
		// Create ESpinner with Configuration
		ESP_PinMode pinModel = espinnerGPIO->getPinModeConf();
		ESPAllOnPinManager::getInstance().updateGPIOFromESPUI(pinModel,
															  parentRef);

		// TODO Update ESpinner if pin Model was set before.
		// Save GPIO Instance in Storage
		ESPinner_Manager::getInstance().push(std::move(espinnerGPIO));
		// Save GPIO Instance in Storage
		// TODO Call to Utils in order to recheck Selectors created in other
		// Containers.
	} else {
		char *backgroundStyle = getBackground(DANGER_COLOR);
		ESPUI.setElementStyle(SaveButtonRef, backgroundStyle);
	}
}

void saveGPIO_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		uint16_t GPIOSelectorRef =
			searchByLabel(parentRef, GPIO_PINSELECTOR_LABEL);
		if (GPIOSelectorRef != 0) {
			String GPIOSelector_value =
				ESPUI.getControl(GPIOSelectorRef)->value;
			// If Text Input --> Check if number
			if (isNumericAndInRange(GPIOSelector_value)) {

				// modify ESPinnerSelectors and Save
				saveButtonGPIOCheck(parentRef);
				char *backgroundStyle = getBackground(SELECTED_COLOR);
				ESPUI.setPanelStyle(parentRef, backgroundStyle);
			} else {
				char *backgroundStyle = getBackground(PENDING_COLOR);
				ESPUI.setPanelStyle(parentRef, backgroundStyle);
			}
		}
	}
}

void GPIOSelector_callback(Control *sender, int type) {
	DUMP("CB: id(", sender->id);
	DUMP(") Type(", type);
	DUMP(") '", sender->label);
	DUMP("' = ", sender->value);

	if (isNumericAndInRange(sender->value)) {

		// Remove Select GPIO LABEL
		uint16_t parentRef = getParentId(elementToParentMap, sender->id);
		// Check Selector in order to create ESPinner and remove Pin from
		// GPIO PinManager
		uint16_t selectLabelRef = searchByLabel(parentRef, GPIO_SELECT_LABEL);
		DUMPLN("Selector Label ", selectLabelRef);
		if (selectLabelRef != 0) {
			ESPUI.removeControl(selectLabelRef);
			removeValueFromMap(elementToParentMap, selectLabelRef);
			saveButtonGPIOCheck(parentRef);
		}

		// Change Selector Control with Text Input for numbers
		uint16_t GPIOSelectorRef =
			searchByLabel(parentRef, GPIO_PINSELECTOR_LABEL);
		char *backgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(GPIOSelectorRef, backgroundStyle);

		if (GPIOSelectorRef != 0) {
			// ESPUI.removeControl(GPIOSelectorRef);
			// removeValueFromMap(elementToParentMap, GPIOSelectorRef);
		}
	} else {
		char *backgroundStyle = getBackground(DANGER_COLOR);
		uint16_t parentRef = getParentId(elementToParentMap, sender->id);
		uint16_t GPIOSelectorRef =
			searchByLabel(parentRef, GPIO_PINSELECTOR_LABEL);
		if (GPIOSelectorRef != 0) {
			ESPUI.setElementStyle(GPIOSelectorRef, backgroundStyle);
		}
	}
}

void GPIO_Selector(uint16_t PIN_ptr) {
	GUI_GPIOSetLabel(PIN_ptr, GPIO_SELECT_LABEL, GPIO_SELECT_VALUE);
	GUI_GPIOSelector(PIN_ptr, GPIO_PINSELECTOR_LABEL, GPIO_PINSELECTOR_VALUE,
					 GPIOSelector_callback);
}

void removeGPIO_callback(Control *sender, int type) {
	if (type == B_UP) {
		debugCallback(sender, type);
		uint16_t parentRef = getParentId(elementToParentMap, sender->id);

		std::vector<uint16_t> childrenIds =
			getChildrenIds(elementToParentMap, parentRef);
		for (uint16_t childControllerId : childrenIds) {
			String espinner_value =
				String(ESPUI.getControl(childControllerId)->value);
			String espinner_label =
				String(ESPUI.getControl(childControllerId)->label);

			// Review which Pin is disconnected in order to detach from
			// ESPinnerManager
			if (espinner_label == GPIO_PINSELECTOR_LABEL) {

				DUMP("DETACH PIN ", espinner_value.toInt())
				ESPAllOnPinManager::getInstance().detach(
					espinner_value.toInt());
			}
		}
		removeElement_callback(sender, type);
	}
}

void GPIO_UI(uint16_t GPIO_ptr) {
	int numElements = sizeof(GPIO_mods) / sizeof(GPIO_mods[0]);

	auto GPIOMode_selector = ESPUI.addControl(
		ControlType::Select, GPIO_MODESELECTOR_LABEL, GPIO_mods[0].name,
		ControlColor::Wetasphalt, GPIO_ptr, createGPIOMod_callback);
	for (int i = 0; i < numElements; i++) {
		ESPUI.addControl(ControlType::Option, GPIO_mods[i].name.c_str(),
						 GPIO_mods[i].name, None, GPIOMode_selector);
	}

	addElementWithParent(elementToParentMap, GPIOMode_selector,
						 GPIO_ptr); // Add INPUT/OUTPUT Selector to parent

	// Num Pin selector
	GPIO_Selector(GPIO_ptr);

	GUIButtons_Elements(GPIO_ptr, GPIO_SAVE_LABEL, GPIO_SAVE_VALUE,
						REMOVEESPINNER_LABEL, REMOVEESPINNER_VALUE,
						saveGPIO_callback, removeGPIO_callback);
}

void GPIO_UIFromESPinner(uint16_t tab_ptr) {

	uint16_t GPIOPIN_selector = ESPUI.addControl(
		ControlType::Select, GPIO_PINSELECTOR_LABEL, GPIO_PINSELECTOR_VALUE,
		ControlColor::Wetasphalt, tab_ptr, GPIOSelector_callback);

	// for (const auto &pair : ESPAllOnPinManager::getInstance().gpioLabels) {
	// TODO Pins attached not included, neither broken not used with
	// Wifi or other issue.
	/* if (!ESPAllOnPinManager::getInstance().isPinAttached(pair.first)) {
		ESPUI.addControl(ControlType::Option,
						 getLabelFromPinManager(pair.first),
						 String(pair.first), None, GPIOPIN_selector);
	} */
	// }
	addElementWithParent(elementToParentMap, GPIOPIN_selector,
						 tab_ptr); // Add GPIO Selector to parent

	int numElements = sizeof(GPIO_mods) / sizeof(GPIO_mods[0]);

	auto GPIOMode_selector = ESPUI.addControl(
		ControlType::Select, GPIO_MODESELECTOR_LABEL, GPIO_mods[0].name,
		ControlColor::Wetasphalt, GPIOPIN_selector, createGPIOMod_callback);
	for (int i = 0; i < numElements; i++) {
		ESPUI.addControl(ControlType::Option, GPIO_mods[i].name.c_str(),
						 GPIO_mods[i].name, None, GPIOMode_selector);
	}

	addElementWithParent(
		elementToParentMap, GPIOMode_selector,
		GPIOPIN_selector); // Add INPUT/OUTPUT Selector to parent

	// GPIO_Selector(GPIOPIN_selector);
	GUIButtons_Elements(GPIOPIN_selector, GPIO_SAVE_LABEL, GPIO_SAVE_VALUE,
						REMOVEESPINNER_LABEL, REMOVEESPINNER_VALUE,
						saveGPIO_callback, removeElement_callback);
}

#endif