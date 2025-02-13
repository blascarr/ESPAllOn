#ifndef _ESPINNER_NEOPIXEL_CONTROLS_H
#define _ESPINNER_NEOPIXEL_CONTROLS_H

#include "./ESPinner_NeoPixel.h"

#include "../../manager/ESPinner_Manager.h"

void createNP_callback(Control *sender, int type) {}
void saveNP_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		// If Text Input --> Check if number, modify ESPinnerSelectors and Save
		saveButtonCheck(parentRef, NEOPIXEL_PININPUT_LABEL,
						NEOPIXEL_SAVE_LABEL);
	}
}

/*
 *	NP_ModelSelector
 *	Select_NP
 *	NP_Save
 *	NP_Remove
 */

void NP_Selector(uint16_t NP_ptr) {
	GUI_GPIOSetLabel(NP_ptr, NEOPIXEL_SELECT_LABEL, NEOPIXEL_SELECT_VALUE);
	GUI_GPIOSelector(NP_ptr, NEOPIXEL_PINSELECTOR_LABEL,
					 NEOPIXEL_PINSELECTOR_VALUE, createNP_callback);
}

void NP_UI(uint16_t NP_ptr) {
	NP_Selector(NP_ptr);
	GUIButtons_Elements(NP_ptr, NEOPIXEL_SAVE_LABEL, NEOPIXEL_SAVE_VALUE,
						NEOPIXEL_REMOVE_LABEL, NEOPIXEL_REMOVE_VALUE,
						saveNP_callback, removeElement_callback);
}

#endif