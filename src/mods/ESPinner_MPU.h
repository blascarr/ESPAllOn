#ifndef _ESPINNER_MPU_H
#define _ESPINNER_MPU_H

#include "../controllers/ESPinner.h"
#include <ESPUI.h>

class ESPinner_MPU : public ESPinner {
  public:
	ESPinner_MPU(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_MPU() : ESPinner(ESPinner_Mod::MPU) {}
	void setup() override { DUMPSLN("Iniciacion configuración de MPU..."); }
	void update() override { DUMPSLN("Update configuración de MPU..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de MPU...");
	}

	void loader() override { DUMPSLN("Cargando configuración de MPU..."); }
};

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
	GUI_GPIOSetLabel(MPU_ptr, MPU_SELECT_LABEL, MPU_SELECT_VALUE);
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