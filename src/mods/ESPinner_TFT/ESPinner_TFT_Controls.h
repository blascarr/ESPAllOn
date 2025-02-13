#ifndef _ESPINNER_TFT_CONTROLS_H
#define _ESPINNER_TFT_CONTROLS_H

#include "./ESPinner_TFT.h"

#include "../../manager/ESPinner_Manager.h"

void createTFT_callback(Control *sender, int type) {}
void saveTFT_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		// If Text Input --> Check if number, modify ESPinnerSelectors and Save
		saveButtonCheck(parentRef, TFT_PININPUT_LABEL, TFT_SAVE_LABEL);
	}
}

/*
 *	TFT_ModelSelector
 *	Select_TFT
 *	TFT_Save
 *	TFT_Remove
 */

void TFT_Selector(uint16_t TFT_ptr) {
	GUI_GPIOSetLabel(TFT_ptr, TFT_SELECT_LABEL, TFT_SELECT_VALUE);
	GUI_GPIOSelector(TFT_ptr, TFT_PINSELECTOR_LABEL, TFT_PINSELECTOR_VALUE,
					 createTFT_callback);
}

void TFT_UI(uint16_t TFT_ptr) {
	TFT_Selector(TFT_ptr);
	GUIButtons_Elements(TFT_ptr, TFT_SAVE_LABEL, TFT_SAVE_VALUE,
						TFT_REMOVE_LABEL, TFT_REMOVE_VALUE, saveTFT_callback,
						removeElement_callback);
}

#endif