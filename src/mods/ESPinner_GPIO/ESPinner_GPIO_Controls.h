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

void gpio_action(uint16_t parentRef, uint16_t GPIOSelectorRef) {
	auto espinnerGPIO = std::make_unique<ESPinner_GPIO>();
	std::vector<uint16_t> childrenIds =
		getChildrenIds(elementToParentMap, parentRef);
	for (uint16_t childControllerId : childrenIds) {
		String espinner_value =
			String(ESPUI.getControl(childControllerId)->value);
		String espinner_label =
			String(ESPUI.getControl(childControllerId)->label);

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
														  GPIOSelectorRef);
	ESPinner_Manager::getInstance().push(std::move(espinnerGPIO));

	ESPinner_Manager::getInstance().saveESPinnersInStorage();
}

void GPIOSwitcher_callback(Control *sender, int type) {
	ESPinner_Manager::getInstance().debugController();

	int switchValue = String(sender->value).toInt();
	int parentRef =
		ESPinner_Manager::getInstance().findBySelectorId(sender->id);

	uint16_t PINSelectorRef = searchByLabel(parentRef, GPIO_PINSELECTOR_LABEL);
	uint8_t associatedPin =
		ESPAllOnPinManager::getInstance().getCurrentReference(PINSelectorRef);

	if (associatedPin != 0) {
		if (switchValue == 1) {
			digitalWrite(associatedPin, HIGH);
		} else {
			digitalWrite(associatedPin, LOW);
		}
	}
}

uint16_t GPIO_Controller(String ID_LABEL) {

	uint16_t controllerTabRef = getTab(TabType::ControllerTab);
	uint16_t GPIOPIN_ID = ESPUI.addControl(
		ControlType::Text, GPIO_SWITCH_ID_LABEL, ID_LABEL.c_str(),
		ControlColor::Wetasphalt, controllerTabRef, debugCallback);
	ESPUI.getControl(GPIOPIN_ID)->enabled = false;

	uint16_t GPIOPIN_Switch = ESPUI.addControl(
		ControlType::Switcher, GPIO_SWITCH_LABEL, "0", ControlColor::Wetasphalt,
		GPIOPIN_ID, GPIOSwitcher_callback);
	return GPIOPIN_Switch;
}

void saveGPIO_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);

	if (type == B_UP) {
		uint16_t GPIOSelectorRef =
			searchByLabel(parentRef, GPIO_PINSELECTOR_LABEL);
		uint16_t GPIOModeRef =
			searchByLabel(parentRef, GPIO_MODESELECTOR_LABEL);
		uint16_t GPIOIDRef = searchByLabel(parentRef, ESPINNERID_LABEL);
		if (GPIOSelectorRef != 0) {
			String GPIOSelector_value =
				ESPUI.getControl(GPIOSelectorRef)->value;
			// If Text Input --> Check if number
			if (isNumericAndInRange(GPIOSelector_value, GPIOSelectorRef)) {

				// modify ESPinnerSelectors and Save
				saveButtonGPIOCheck(parentRef, GPIO_PINSELECTOR_LABEL,
									gpio_action);

				uint16_t switchId =
					GPIO_Controller(ESPUI.getControl(GPIOIDRef)->value);
				ESPinner_Manager::getInstance().addControllerRelation(parentRef,
																	  switchId);
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
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (isNumericAndInRange(sender->value, parentRef)) {

		// Remove Select GPIO LABEL
		uint16_t parentRef = getParentId(elementToParentMap, sender->id);
		// Check Selector in order to create ESPinner and remove Pin from
		// GPIO PinManager
		uint16_t selectLabelRef = searchByLabel(parentRef, GPIO_SELECT_LABEL);

		if (selectLabelRef != 0) {
			ESPUI.removeControl(selectLabelRef);
			removeValueFromMap(elementToParentMap, selectLabelRef);
			saveButtonGPIOCheck(parentRef, GPIO_PINSELECTOR_LABEL, gpio_action);
		}

		// Change Selector Control with Text Input for numbers
		uint16_t GPIOSelectorRef =
			searchByLabel(parentRef, GPIO_PINSELECTOR_LABEL);
		char *backgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(GPIOSelectorRef, backgroundStyle);

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
			detachRemovedPIN(GPIO_PINSELECTOR_LABEL, espinner_label,
							 espinner_value);
		}
		removeElement_callback(sender, type);
	}
}

uint16_t GPIO_ModeSelector(uint16_t GPIO_ptr) {
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
	return GPIOMode_selector;
}

void GPIO_UI(uint16_t GPIO_ptr) {
	GPIO_ModeSelector(GPIO_ptr);

	// Num Pin selector
	GPIO_Selector(GPIO_ptr);

	GUIButtons_Elements(GPIO_ptr, GPIO_SAVE_LABEL, GPIO_SAVE_VALUE,
						REMOVEESPINNER_LABEL, REMOVEESPINNER_VALUE,
						saveGPIO_callback, removeGPIO_callback);
}

void ESPinner_GPIO::implement() {
	uint16_t parentRef = getTab(TabType::BasicTab);

	uint16_t GPIOPIN_selector = ESPUI.addControl(
		ControlType::Text, ESPINNERID_LABEL, ESPinner_GPIO::getID(),
		ControlColor::Wetasphalt, parentRef, GPIOSelector_callback);
	ESPUI.getControl(GPIOPIN_selector)->enabled = false;
	addElementWithParent(elementToParentMap, GPIOPIN_selector,
						 GPIOPIN_selector);

	uint16_t gpioMode_ref = GPIO_ModeSelector(GPIOPIN_selector);
	ESPUI.getControl(gpioMode_ref)->value = ESPinner_GPIO::getGPIOMode_JSON();

	String gpio = String(ESPinner_GPIO::getGPIO());
	uint16_t gpio_ref =
		GUI_GPIOSelector(GPIOPIN_selector, GPIO_PINSELECTOR_LABEL, gpio.c_str(),
						 GPIOSelector_callback);

	GUIButtons_Elements(GPIOPIN_selector, GPIO_SAVE_LABEL, GPIO_SAVE_VALUE,
						REMOVEESPINNER_LABEL, REMOVEESPINNER_VALUE,
						saveGPIO_callback, removeGPIO_callback);

	ESPAllOnPinManager::getInstance().setPinControlRelation(
		ESPinner_GPIO::getGPIO(), gpio_ref);

	//------------------------------------------------//
	//-------- ATTACH GPIO AND PINMODE CONFIG --------//
	//------------------------------------------------//
	if (ESPinner_GPIO::getGPIOMode() == GPIOMode::Input) {
		ESP_PinMode pinModel = ESP_PinMode(ESPinner_GPIO::getGPIO(),
										   OutputPin(), PinType::BusDigital);
		ESPAllOnPinManager::getInstance().attach(pinModel);
	}
	if (ESPinner_GPIO::getGPIOMode() == GPIOMode::Output) {
		ESP_PinMode pinModel = ESP_PinMode(ESPinner_GPIO::getGPIO(), InputPin(),
										   PinType::BusDigital);
		ESPAllOnPinManager::getInstance().attach(pinModel);
	}

	uint16_t switchId = GPIO_Controller(ESPinner_GPIO::getID());
	ESPinner_Manager::getInstance().addControllerRelation(GPIOPIN_selector,
														  switchId);
	ESPinner_Manager::getInstance().debugController();
}
#endif