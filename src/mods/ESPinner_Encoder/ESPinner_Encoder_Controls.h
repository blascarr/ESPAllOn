#ifndef _ESPINNER_ENCODER_CONTROLS_H
#define _ESPINNER_ENCODER_CONTROLS_H

#include "./ESPinner_Encoder.h"

#include "../../manager/ESPinner_Manager.h"

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
	GUI_setLabel(PIN_ptr, ENCODER_SELECT_LABEL, ENCODER_SELECT_VALUE);
	GUI_TextField(PIN_ptr, ENCODER_PINSELECTOR_LABEL, ENCODER_PINSELECTOR_VALUE,
				  createEncoder_callback);
}

void Encoder_UI(uint16_t Encoder_ptr) {
	Encoder_Selector(Encoder_ptr);
	GUIButtons_Elements(Encoder_ptr, ENCODER_SAVE_LABEL, ENCODER_SAVE_VALUE,
						ENCODER_REMOVE_LABEL, ENCODER_REMOVE_VALUE,
						saveEncoder_callback, removeElement_callback);
}

#endif