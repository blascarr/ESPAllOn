#ifndef _ESPINNER_ENCODER_H
#define _ESPINNER_ENCODER_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_Encoder : public ESPinner {
  public:
	ESPinner_Encoder(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_Encoder() : ESPinner(ESPinner_Mod::Encoder) {}
	void setup() override {
		DUMPSLN("Iniciacion configuración de GPIO...");
		// pinManager->isPinOK(12);
	}
	void update() override { DUMPSLN("Update configuración de GPIO..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de GPIO...");
	}

	void loader() override { DUMPSLN("Cargando configuración de GPIO..."); }
};

void createEncoder_callback(Control *sender, int type) {}
/*
 *	Encoder_ModeSelector
 *	Encoder_Pin1
 * 	Encoder_pin2
 *	Encoder_Save
 *	Encoder_Remove
 */

void saveEncoder_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		// If Text Input --> Check if number, modify ESPinnerSelectors and Save
		saveButtonCheck(parentRef, "GPIO_PinInput", "GPIOSave");
	}
}

void Encoder_Selector(uint16_t PIN_ptr) {
	GUI_GPIOSetLabel(PIN_ptr, "Select_Encoder_GPIO", "Select Encoder");
	GUI_GPIOSelector(PIN_ptr, "Encoder_PinSelector", "0",
					 createEncoder_callback);
}

void Encoder_UI(uint16_t Encoder_ptr) {
	Encoder_Selector(Encoder_ptr);
	GUIButtons_Elements(Encoder_ptr, "GPIOSave", "Save GPIO", "GPIORemove",
						"Remove", saveEncoder_callback, removeElement_callback);
}
#endif