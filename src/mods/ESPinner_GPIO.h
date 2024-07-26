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
void createPIN_callback(Control *sender, int type) {
	DUMP("CB: id(", sender->id);
	DUMP(") Type(", type);
	DUMP(") '", sender->label);
	DUMP("' = ", sender->value);
}

void GPIO_Selector(uint16_t PIN_ptr) {
	// Access to current configuration in Board ESP
	// Just only numbers defined in PinManager Object
	// Remove broken and not permitted GPIOs

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
	uint16_t GPIOAdd_selector = ESPUI.addControl(
		ControlType::Button, "GPIOSave", "Save", ControlColor::Alizarin,
		GPIO_ptr, saveElement_callback);
	uint16_t GPIORemove_selector = ESPUI.addControl(
		ControlType::Button, "GPIORemove", "Remove", ControlColor::Alizarin,
		GPIO_ptr, removeElement_callback);

	addElementWithParent(elementToParentMap, GPIOAdd_selector,
						 GPIO_ptr); // Add Save Button to parent
	addElementWithParent(elementToParentMap, GPIORemove_selector,
						 GPIO_ptr); // Add Remove Button to parent
	debugMap(elementToParentMap);
}
#endif