#ifndef _ESPINNER_LCD_H
#define _ESPINNER_LCD_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_LCD : public ESPinner {
  public:
	ESPinner_LCD(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_LCD() : ESPinner(ESPinner_Mod::LCD) {}
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

void createLCD_callback(Control *sender, int type) {}
void saveLCD_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		// If Text Input --> Check if number, modify ESPinnerSelectors and Save
		saveButtonCheck(parentRef, "LCD_PinInput", "LCD_Save");
	}
}

/*
 *	LCD_ModelSelector
 *	Select_LCD
 *	LCD_Save
 *	LCD_Remove
 */

void LCD_Selector(uint16_t LCD_ptr) {
	GUI_GPIOSetLabel(LCD_ptr, "Select_LCD_GPIO", "Select LCD");
	GUI_GPIOSelector(LCD_ptr, "LCD_PinSelector", "0", createLCD_callback);
}

void LCD_UI(uint16_t LCD_ptr) {
	LCD_Selector(LCD_ptr);
	GUIButtons_Elements(LCD_ptr, "LCD_Save", "Save LCD", "LCD_Remove", "Remove",
						saveLCD_callback, removeElement_callback);
}

#endif