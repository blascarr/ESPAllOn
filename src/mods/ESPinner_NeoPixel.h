#ifndef _ESPINNER_NEOPIXEL_H
#define _ESPINNER_NEOPIXEL_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_NEOPIXEL : public ESPinner {
  public:
	ESPinner_NEOPIXEL(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_NEOPIXEL() : ESPinner(ESPinner_Mod::NeoPixel) {}
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

void createNP_callback(Control *sender, int type) {}
void saveNP_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		// If Text Input --> Check if number, modify ESPinnerSelectors and Save
		saveButtonCheck(parentRef, "NP_PinInput", "NP_Save");
	}
}

/*
 *	NP_ModelSelector
 *	Select_NP
 *	NP_Save
 *	NP_Remove
 */

void NP_Selector(uint16_t NP_ptr) {
	GUI_GPIOSetLabel(NP_ptr, "Select_NP_GPIO", "Select NP");
	GUI_GPIOSelector(NP_ptr, "NP_PinSelector", "0", createNP_callback);
}

void NP_UI(uint16_t NP_ptr) {
	NP_Selector(NP_ptr);
	GUIButtons_Elements(NP_ptr, "NP_Save", "Save NeoPixel", "NP_Remove",
						"Remove", saveNP_callback, removeElement_callback);
}

#endif