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
	{ESPinner_GPIOType::ESPINNER_INPUT, "INPUT"},
	{ESPinner_GPIOType::ESPINNER_OUTPUT, "OUTPUT"}};
class ESPinner_GPIO : public ESPinner {
  public:
	uint8_t gpio;
	ESPinner_GPIOType GPIO_mode;

	ESPinner_GPIO(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_GPIO() : ESPinner(ESPinner_Mod::GPIO) {}
	void setup() override {
		DUMPSLN("Iniciacion configuración de GPIO...");
		// pinManager->isPinOK(12);
	}
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

void createGPIO_callback(Control *sender, int type) {}
void saveGPIO_callback(Control *sender, int type) {
	if (type == B_UP) {
	}
}

void createPIN_callback(Control *sender, int type) {
	DUMP("CB: id(", sender->id);
	DUMP(") Type(", type);
	DUMP(") '", sender->label);
	DUMP("' = ", sender->value);

	// Remove Select GPIO LABEL
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	uint16_t selectLabelRef = searchByLabel(parentRef, "Select_GPIO");
	if (selectLabelRef != 0) {
		ESPUI.removeControl(selectLabelRef);
		removeValueFromMap(elementToParentMap, selectLabelRef);
	}

	// Change Selector Control with Text Input for numbers
	uint16_t GPIOSelectorRef = searchByLabel(parentRef, "GPIO_PinSelector");
	DUMPLN("Selector Ref ", selectLabelRef);
	if (GPIOSelectorRef != 0) {
		DUMPLN("Selector Ref ", selectLabelRef);
		uint16_t GPIOPIN_input = ESPUI.addControl(
			ControlType::Text, "GPIO_PinInput",
			String(ESPUI.getControl(GPIOSelectorRef)->value),
			ControlColor::Wetasphalt, parentRef, saveGPIO_callback);
		addElementWithParent(elementToParentMap, GPIOPIN_input,
							 parentRef); // Add GPIO Selector Input to parent
		ESPUI.removeControl(GPIOSelectorRef);
		removeValueFromMap(elementToParentMap, GPIOSelectorRef);
	}
}

void GPIO_Selector(uint16_t PIN_ptr) {
	// Access to current configuration in Board ESP
	// Just only numbers defined in PinManager Object
	// Remove broken and not permitted GPIOs

	uint16_t GPIOLabel_selector =
		ESPUI.addControl(ControlType::Label, "Select_GPIO", "Select GPIO",
						 ControlColor::Carrot, PIN_ptr);
	addElementWithParent(elementToParentMap, GPIOLabel_selector,
						 PIN_ptr); // Add GPIO Label to parent
	uint16_t GPIOPIN_selector =
		ESPUI.addControl(ControlType::Select, "GPIO_PinSelector", "0",
						 ControlColor::Wetasphalt, PIN_ptr, createPIN_callback);
	for (int i = 0; i < ESP_BoardConf::NUM_PINS; i++) {
		if (!ESPAllOnPinManager::getInstance().isPinAttached(i)) {
			ESPUI.addControl(ControlType::Option, getLabelForPin(i), String(i),
							 None, GPIOPIN_selector);
		}
	}
	addElementWithParent(elementToParentMap, GPIOPIN_selector,
						 PIN_ptr); // Add GPIO Selector to parent
}

void GPIO_UI(uint16_t GPIO_ptr) {
	int numElements = sizeof(GPIO_mods) / sizeof(GPIO_mods[0]);

	auto GPIOMode_selector = ESPUI.addControl(
		ControlType::Select, "GPIO_ModeSelector", GPIO_mods[0].name,
		ControlColor::Wetasphalt, GPIO_ptr, createGPIO_callback);
	for (int i = 0; i < numElements; i++) {
		ESPUI.addControl(ControlType::Option, GPIO_mods[i].name.c_str(),
						 GPIO_mods[i].name, None, GPIOMode_selector);
	}

	addElementWithParent(elementToParentMap, GPIOMode_selector,
						 GPIO_ptr); // Add INPUT/OUTPUT Selector to parent

	// Num Pin selector
	GPIO_Selector(GPIO_ptr);

	// Remove Element
	uint16_t GPIOAdd_selector =
		ESPUI.addControl(ControlType::Button, "GPIOSave", "Save GPIO",
						 ControlColor::Alizarin, GPIO_ptr, saveGPIO_callback);
	uint16_t GPIORemove_selector = ESPUI.addControl(
		ControlType::Button, "GPIORemove", "Remove", ControlColor::Alizarin,
		GPIO_ptr, removeElement_callback);

	addElementWithParent(elementToParentMap, GPIOAdd_selector,
						 GPIO_ptr); // Add Save Button to parent
	addElementWithParent(elementToParentMap, GPIORemove_selector,
						 GPIO_ptr); // Add Remove Button to parent
									// debugMap(elementToParentMap);
}
#endif