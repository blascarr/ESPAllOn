#ifndef _ESPINNER_GPIO_H
#define _ESPINNER_GPIO_H

#include "../ESPinner.h"
#include <ESPUI.h>

enum class ESPinner_GPIOType { ESPINNER_INPUT, ESPINNER_OUTPUT };

struct ESPinner_GPIOMode {
	ESPinner_GPIOType model;
	String name;
};

const ESPinner_GPIOMode GPIO_mods[] = {
	{ESPinner_GPIOType::ESPINNER_INPUT, GPIO_ESPINNERINPUT_LABEL},
	{ESPinner_GPIOType::ESPINNER_OUTPUT, GPIO_ESPINNEROUTPUT_LABEL}};
class ESPinner_GPIO : public ESPinner {
  public:
	uint8_t gpio;
	ESPinner_GPIOType GPIO_mode;

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
	void setGPIOMode(ESPinner_GPIOType mode) { GPIO_mode = mode; }
	ESPinner_GPIOType getGPIOMode() { return GPIO_mode; }
};

void createGPIOMod_callback(Control *sender, int type) {}

void saveButtonGPIOCheck(uint16_t parentRef) {
	uint16_t GPIOTextInputRef = searchByLabel(parentRef, GPIO_PININPUT_LABEL);
	uint16_t GPIOSelectorRef = searchByLabel(parentRef, GPIO_PINSELECTOR_LABEL);

	String GPIOSelector_value;
	if (GPIOSelectorRef != 0) {
		GPIOSelector_value = ESPUI.getControl(GPIOSelectorRef)->value;
		return;
	}
	if (GPIOTextInputRef != 0) {
		GPIOSelector_value = ESPUI.getControl(GPIOTextInputRef)->value;
	}
	uint16_t SaveButtonRef = searchByLabel(parentRef, GPIO_SAVE_LABEL);

	if (GPIOSelector_value == "0" || isNumericString(GPIOSelector_value)) {
		char *backgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(SaveButtonRef, backgroundStyle);
		// Save ESPINNER

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
	uint16_t selectLabelRef = searchByLabel(parentRef, GPIO_SELECT_LABEL);
	if (selectLabelRef != 0) {
		ESPUI.removeControl(selectLabelRef);
		removeValueFromMap(elementToParentMap, selectLabelRef);
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