#ifndef _ESPINNER_MPU_CONTROLS_H
#define _ESPINNER_MPU_CONTROLS_H

#include "./ESPinner_MPU.h"

#include "../../manager/ESPinner_Manager.h"

void createMPU_callback(Control *sender, int type) {}
void saveMPU_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		// If Text Input --> Check if number, modify ESPinnerSelectors and Save
		saveButtonCheck(parentRef, MPU_PININPUT_LABEL, MPU_SAVE_LABEL);
	}
}

/*
 *	MPU_ModelSelector
 *	Select_MPU
 *	MPU_Save
 *	MPU_Remove
 */

void MPU_Selector(uint16_t MPU_ptr) {
	GUI_setLabel(MPU_ptr, MPU_SELECT_LABEL, MPU_SELECT_VALUE);
	GUI_GPIOSelector(MPU_ptr, MPU_PINSELECTOR_LABEL, MPU_PINSELECTOR_VALUE,
					 createMPU_callback);
}

void MPU_UI(uint16_t MPU_ptr) {
	MPU_Selector(MPU_ptr);
	GUIButtons_Elements(MPU_ptr, MPU_SAVE_LABEL, MPU_SAVE_VALUE,
						MPU_REMOVE_LABEL, MPU_REMOVE_VALUE, saveMPU_callback,
						removeElement_callback);
}

#endif