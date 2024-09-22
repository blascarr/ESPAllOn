#ifndef _ESPINNER_GPIO_H
#define _ESPINNER_GPIO_H

#include "../ESPinner.h"
#include <ESPUI.h>

struct ESPinner_GPIOMode {
	GPIOMode model;
	String name;
};

const ESPinner_GPIOMode GPIO_mods[] = {
	{GPIOMode::Input, GPIO_ESPINNERINPUT_LABEL},
	{GPIOMode::Output, GPIO_ESPINNEROUTPUT_LABEL}};
class ESPinner_GPIO : public ESPinner {
  public:
	uint8_t gpio;
	GPIOMode GPIO_mode;

	ESPinner_GPIO(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_GPIO() : ESPinner(ESPinner_Mod::GPIO) {}
	void setup() override { DUMPSLN("Iniciacion configuración de GPIO..."); }
	void update() override { DUMPSLN("Update configuración de GPIO..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de GPIO...");
	}

	void loader() override { DUMPSLN("Cargando configuración de GPIO..."); }

	void setGPIO(uint8_t gpio_pin) { gpio = gpio_pin; }
	uint8_t getGPIO() { return gpio; }
	void setGPIOMode(GPIOMode mode) { GPIO_mode = mode; }
	GPIOMode getGPIOMode() { return GPIO_mode; }
	ESP_PinMode getPinModeConf() {

		switch (GPIO_mode) {
		case GPIOMode::Input: {
			InputPin model = InputPin(false, false);
			ESP_PinMode pinMode = {this->getGPIO(), model, PinType::BusDigital};
			return pinMode;
		}
		case GPIOMode::Output: {
			OutputPin model = OutputPin(false);
			ESP_PinMode pinMode = {this->getGPIO(), model, PinType::BusDigital};
			return pinMode;
		}
		// If no GPIO_Mode is configured, return as Output by default
		default:
			ESP_PinMode pinMode = {this->getGPIO(), OutputPin(false),
								   PinType::BusDigital};
			return pinMode;
		}
	};
};

void createGPIOMod_callback(Control *sender, int type) {}

void saveButtonGPIOCheck(uint16_t parentRef) {
	uint16_t GPIOTextInputRef = searchByLabel(parentRef, GPIO_PININPUT_LABEL);
	uint16_t GPIOSelectorRef = searchByLabel(parentRef, GPIO_PINSELECTOR_LABEL);

	String GPIOSelector_value;
	if (GPIOSelectorRef != 0) {
		GPIOSelector_value = ESPUI.getControl(GPIOSelectorRef)->value;
		// TODO if GPIOSelector_value exists -> Check if ESP_PinMode List
		// related with Container exists. Create new One
	}
	if (GPIOTextInputRef != 0) {
		// TODO if InputRef is checked, we
		// should update the PinModel in PinManager
		GPIOSelector_value = ESPUI.getControl(GPIOTextInputRef)->value;
	}
	uint16_t SaveButtonRef = searchByLabel(parentRef, GPIO_SAVE_LABEL);

	if (isNumericString(GPIOSelector_value)) {
		char *backgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(SaveButtonRef, backgroundStyle);

		// Save ESPINNER
		ESPinner_GPIO espinnerGPIO;
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
				if (espinner_label == GPIO_ESPINNERINPUT_LABEL) {
					espinnerGPIO.setGPIOMode(GPIOMode::Input);
				} else if (espinner_label == GPIO_ESPINNEROUTPUT_LABEL) {
					espinnerGPIO.setGPIOMode(GPIOMode::Output);
				}
			}
			if (espinner_label == GPIO_PININPUT_LABEL ||
				espinner_label == GPIO_PINSELECTOR_LABEL) {
				espinnerGPIO.setGPIO(espinner_value.toInt());
			}
		}
		// Create new ESpinner if model is the first one
		ESP_PinMode pinModel = espinnerGPIO.getPinModeConf();
		ESPAllOnPinManager::getInstance().attach(pinModel);

		// TODO Update ESpinner if pin Model was set before.

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
		// If Text Input --> Check if number, modify ESPinnerSelectors and Save
		saveButtonGPIOCheck(parentRef);
	}
}

void createPIN_callback(Control *sender, int type) {
	DUMP("CB: id(", sender->id);
	DUMP(") Type(", type);
	DUMP(") '", sender->label);
	DUMP("' = ", sender->value);

	// Remove Select GPIO LABEL
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	// Check Selector in order to create ESPinner and remove Pin from GPIO
	// PinManager
	uint16_t selectLabelRef = searchByLabel(parentRef, GPIO_SELECT_LABEL);
	DUMPLN("Selector Label ", selectLabelRef);
	if (selectLabelRef != 0) {
		ESPUI.removeControl(selectLabelRef);
		removeValueFromMap(elementToParentMap, selectLabelRef);
		saveButtonGPIOCheck(parentRef);
	}

	// Change Selector Control with Text Input for numbers
	uint16_t GPIOSelectorRef = searchByLabel(parentRef, GPIO_PINSELECTOR_LABEL);
	if (GPIOSelectorRef != 0) {
		uint16_t GPIOPIN_input = ESPUI.addControl(
			ControlType::Text, GPIO_PININPUT_LABEL,
			String(ESPUI.getControl(GPIOSelectorRef)->value),
			ControlColor::Wetasphalt, parentRef, saveGPIO_callback);
		addElementWithParent(elementToParentMap, GPIOPIN_input,
							 parentRef); // Add GPIO Selector Input to parent
		saveButtonCheck(parentRef, GPIO_PININPUT_LABEL, GPIO_SAVE_LABEL);
		ESPUI.removeControl(GPIOSelectorRef);
		removeValueFromMap(elementToParentMap, GPIOSelectorRef);
	}
}

/*
 *	GPIO_ModeSelector
 *	Select_GPIO
 *	GPIO_PinSelector
 *	GPIOSave
 *	GPIORemove
 */

void GPIO_Selector(uint16_t PIN_ptr) {
	GUI_GPIOSetLabel(PIN_ptr, GPIO_SELECT_LABEL, GPIO_SELECT_VALUE);
	GUI_GPIOSelector(PIN_ptr, GPIO_PINSELECTOR_LABEL, GPIO_PINSELECTOR_VALUE,
					 createPIN_callback);
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
						saveGPIO_callback, removeElement_callback);
}
#endif