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
	void setup() override {
		Serial.print("Iniciacion configuración de GPIO...");
		// pinManager->isPinOK(12);
	}
	void update() override { Serial.print("Update configuración de GPIO..."); }
	void implement() override {
		Serial.print("Implementacion configuración de GPIO...");
	}

	void loader() override {
		Serial.print("Cargando configuración de GPIO...");
	}
};

void createGPIO_callback(Control *sender, int type) {}
void createPIN_callback(Control *sender, int type) {
	Serial.print("CB: id(");
	Serial.print(sender->id);
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
}

void GPIO_Selector(uint16_t PIN_ptr) {
	// Access to current configuration in Board ESP
	// Just only numbers defined in PinManager Object
	// Remove broken and not permitted GPIOs
	uint8_t numElements = 20;

	uint16_t GPIOPIN_selector =
		ESPUI.addControl(ControlType::Select, "Pin Selector", "0",
						 ControlColor::Wetasphalt, PIN_ptr, createPIN_callback);
	for (int i = 0; i < numElements; i++) {

		ESPUI.addControl(ControlType::Option, String(i).c_str(), String(i),
						 None, GPIOPIN_selector);
	}
	// addElementWithParent(elementToParentMap, GPIOPIN_selector, PIN_ptr);
}

void GPIO_UI(uint16_t GPIO_ptr) {
	int numElements = sizeof(GPIO_mods) / sizeof(GPIO_mods[0]);

	auto GPIOMode_selector = ESPUI.addControl(
		ControlType::Select, "GPIO Selector", GPIO_mods[0].name,
		ControlColor::Wetasphalt, GPIO_ptr, createGPIO_callback);
	for (int i = 0; i < numElements; i++) {
		ESPUI.addControl(ControlType::Option, GPIO_mods[i].name.c_str(),
						 GPIO_mods[i].name, None, GPIOMode_selector);
	}

	addElementWithParent(elementToParentMap, GPIOMode_selector, GPIO_ptr);

	// Num Pin selector
	GPIO_Selector(GPIO_ptr);

	// Remove Element
	uint16_t GPIORemove_selector = ESPUI.addControl(
		ControlType::Button, "Remove", "Remove", ControlColor::Alizarin,
		GPIO_ptr, removeElement_callback);
	addElementWithParent(elementToParentMap, GPIORemove_selector, GPIO_ptr);
	debugMap(elementToParentMap);
}
#endif