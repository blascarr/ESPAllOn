#ifndef _ESPINNER_STEPPER_H
#define _ESPINNER_STEPPER_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_Stepper : public ESPinner {
  public:
	ESPinner_Stepper(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_Stepper() : ESPinner(ESPinner_Mod::Stepper) {}
	void setup() override {
		DUMPSLN("Iniciacion configuración de Stepper ...");
	}
	void update() override { DUMPSLN("Update configuración de Stepper ..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de Stepper ...");
	}

	void loader() override { DUMPSLN("Cargando configuración de Stepper ..."); }
};

void saveStepper_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		// If Text Input --> Check if number, modify ESPinnerSelectors and Save
		saveButtonCheck(parentRef, "GPIO_PinInput", "GPIOSave");
	}
}

/*
 *	Stepper_ModeSelector
 *	Select_Stepper_P1
 *	Stepper_Save
 *	Stepper_Remove
 */

void Stepper_Selector(uint16_t PIN_ptr) {
	GUI_GPIOSetLabel(PIN_ptr, "Select_Stepper_GPIO", "Select Stepper");
	GUI_GPIOSelector(PIN_ptr, "Stepper_Selector", "0", createDC_callback);
}

void Stepper_UI(uint16_t Stepper_ptr) {
	Stepper_Selector(Stepper_ptr);
	GUIButtons_Elements(Stepper_ptr, "GPIOSave", "Save GPIO", "GPIORemove",
						"Remove", saveStepper_callback, removeElement_callback);
}

#endif