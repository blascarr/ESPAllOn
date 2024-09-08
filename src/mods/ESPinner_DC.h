#ifndef _ESPINNER_DC_H
#define _ESPINNER_DC_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_DC : public ESPinner {
  public:
	ESPinner_DC(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_DC() : ESPinner(ESPinner_Mod::DC) {}
	void setup() override { DUMPSLN("Iniciacion configuración de DC..."); }
	void update() override { DUMPSLN("Update configuración de DC..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de DC...");
	}

	void loader() override { DUMPSLN("Cargando configuración de DC..."); }
};

void createDC_callback(Control *sender, int type) {}
/*
 *	DC_ModeSelector
 *	DC_Pin1
 * 	DC_pin2
 *	DC_Save
 *	DC_Remove
 */

void saveDC_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		// If Text Input --> Check if number, modify ESPinnerSelectors and Save
		saveButtonCheck(parentRef, DC_PININPUT_LABEL, DC_SAVE_LABEL);
	}
}

void DC_Selector(uint16_t PIN_ptr) {
	GUI_GPIOSetLabel(PIN_ptr, DC_SELECT_LABEL, DC_SELECT_VALUE);
	GUI_GPIOSelector(PIN_ptr, DC_PINSELECTOR_LABEL, DC_PINSELECTOR_VALUE,
					 createDC_callback);
}

void DC_UI(uint16_t DC_ptr) {
	DC_Selector(DC_ptr);
	GUIButtons_Elements(DC_ptr, DC_SAVE_LABEL, DC_SAVE_VALUE, DC_REMOVE_LABEL,
						DC_REMOVE_VALUE, saveDC_callback,
						removeElement_callback);
}
#endif