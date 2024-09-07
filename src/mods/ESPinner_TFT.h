#ifndef _ESPINNER_TFT_H
#define _ESPINNER_TFT_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_TFT : public ESPinner {
  public:
	ESPinner_TFT(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_TFT() : ESPinner(ESPinner_Mod::TFT) {}
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

void createTFT_callback(Control *sender, int type) {}
void saveTFT_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		// If Text Input --> Check if number, modify ESPinnerSelectors and Save
		saveButtonCheck(parentRef, "TFT_PinInput", "TFT_Save");
	}
}

/*
 *	TFT_ModelSelector
 *	Select_TFT
 *	TFT_Save
 *	TFT_Remove
 */

void TFT_Selector(uint16_t TFT_ptr) {
	GUI_GPIOSetLabel(TFT_ptr, "Select_TFT_GPIO", "Select TFT");
	GUI_GPIOSelector(TFT_ptr, "TFT_PinSelector", "0", createTFT_callback);
}

void TFT_UI(uint16_t TFT_ptr) {
	TFT_Selector(TFT_ptr);
	GUIButtons_Elements(TFT_ptr, "TFT_Save", "Save TFT", "TFT_Remove", "Remove",
						saveTFT_callback, removeElement_callback);
}

#endif