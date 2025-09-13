#ifndef _ESPALLONGUI_H
#define _ESPALLONGUI_H
#include "../../utils.h"
#include "../ESPAllOnPinManager.h"
#include <Arduino.h>
#include <ESPUI.h>

uint16_t GUI_GPIOSelector(uint16_t parentRef, const char *GPIOLabel,
						  const char *GPIOValue, UICallback SelectorCallback) {
	// Access to current configuration in Board ESP
	// Just only numbers defined in PinManager Object
	// Remove broken and not permitted GPIOs

	uint16_t GPIOPIN_selector =
		ESPUI.addControl(ControlType::Text, GPIOLabel, GPIOValue,
						 ControlColor::Wetasphalt, parentRef, SelectorCallback);

	addElementWithParent(elementToParentMap, GPIOPIN_selector,
						 parentRef); // Add GPIO Selector to parent
	return GPIOPIN_selector;
}

uint16_t GUI_TextField(uint16_t parentRef, const char *label, const char *value,
					   UICallback SelectorCallback) {

	uint16_t textfield =
		ESPUI.addControl(ControlType::Text, label, value,
						 ControlColor::Wetasphalt, parentRef, SelectorCallback);

	addElementWithParent(elementToParentMap, textfield,
						 parentRef); // Add Textfield to parent
	return textfield;
}

#endif