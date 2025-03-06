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
#endif