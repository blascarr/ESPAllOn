#ifndef _ESPINNER_ENCODER_H
#define _ESPINNER_ENCODER_H

#include "../controllers/ESPinner.h"
#include <ESPUI.h>

class ESPinner_Encoder : public ESPinner {
  public:
	ESPinner_Encoder(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_Encoder() : ESPinner(ESPinner_Mod::Encoder) {}
	void setup() override { DUMPSLN("Iniciacion configuración de ENCODER..."); }
	void update() override { DUMPSLN("Update configuración de ENCODER..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de ENCODER...");
	}

	void loader() override { DUMPSLN("Cargando configuración de ENCODER..."); }
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
		saveButtonCheck(parentRef, ENCODER_PININPUT_LABEL,
						ENCODER_PININPUT_VALUE);
	}
}

void Encoder_Selector(uint16_t PIN_ptr) {
	GUI_GPIOSetLabel(PIN_ptr, ENCODER_SELECT_LABEL, ENCODER_SELECT_VALUE);
	GUI_GPIOSelector(PIN_ptr, ENCODER_PINSELECTOR_LABEL,
					 ENCODER_PINSELECTOR_VALUE, createEncoder_callback);
}

void Encoder_UI(uint16_t Encoder_ptr) {
	Encoder_Selector(Encoder_ptr);
	GUIButtons_Elements(Encoder_ptr, ENCODER_SAVE_LABEL, ENCODER_SAVE_VALUE,
						ENCODER_REMOVE_LABEL, ENCODER_REMOVE_VALUE,
						saveEncoder_callback, removeElement_callback);
}
#endif