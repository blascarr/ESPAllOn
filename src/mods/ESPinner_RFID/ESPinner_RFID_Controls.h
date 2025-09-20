#ifndef _ESPINNER_RFID_CONTROLS_H
#define _ESPINNER_RFID_CONTROLS_H

#include "./ESPinner_RFID.h"

#include "../../manager/ESPinner_Manager.h"

void createRFID_callback(Control *sender, int type) {}
/*
 *	RFID_ModeSelector
 *	RFID_Pin1
 * 	RFID_pin2
 *	RFID_Save
 *	RFID_Remove
 */

void saveRFID_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		// If Text Input --> Check if number, modify ESPinnerSelectors and Save
		saveButtonCheck(parentRef, RFID_PININPUT_LABEL, RFID_SAVE_LABEL);
	}
}

void RFID_Selector(uint16_t PIN_ptr) {
	GUI_setLabel(PIN_ptr, RFID_SELECT_LABEL, RFID_SELECT_VALUE);
	GUI_GPIOSelector(PIN_ptr, RFID_PINSELECTOR_LABEL, RFID_PINSELECTOR_VALUE,
					 createRFID_callback);
}

void RFID_UI(uint16_t RFID_ptr) {
	RFID_Selector(RFID_ptr);
	GUIButtons_Elements(RFID_ptr, RFID_SAVE_LABEL, RFID_SAVE_VALUE,
						RFID_REMOVE_LABEL, RFID_REMOVE_VALUE, saveRFID_callback,
						removeElement_callback);
}

#endif