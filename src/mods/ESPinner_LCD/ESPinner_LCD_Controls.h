#ifndef _ESPINNER_LCD_CONTROLS_H
#define _ESPINNER_LCD_CONTROLS_H

#include "./ESPinner_LCD.h"

#include "../../manager/ESPinner_Manager.h"

void createLCD_callback(Control *sender, int type) {}
void saveLCD_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		// If Text Input --> Check if number, modify ESPinnerSelectors and Save
		saveButtonCheck(parentRef, LCD_PININPUT_LABEL, LCD_SAVE_LABEL);
	}
}

/*
 *	LCD_ModelSelector
 *	Select_LCD
 *	LCD_Save
 *	LCD_Remove
 */

void LCD_Selector(uint16_t LCD_ptr) {
	GUI_setLabel(LCD_ptr, "Select_LCD_GPIO", "Select LCD");
	GUI_GPIOSelector(LCD_ptr, LCD_PINSELECTOR_LABEL, LCD_PINSELECTOR_VALUE,
					 createLCD_callback);
}

void LCD_UI(uint16_t LCD_ptr) {
	LCD_Selector(LCD_ptr);
	GUIButtons_Elements(LCD_ptr, LCD_SAVE_LABEL, LCD_SAVE_VALUE,
						LCD_REMOVE_LABEL, LCD_REMOVE_VALUE, saveLCD_callback,
						removeElement_callback);
}

#endif