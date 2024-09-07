#ifndef _ESPALLONGUI_H
#define _ESPALLONGUI_H

void GUI_GPIOSelector(uint16_t parentRef, const char *GPIOLabel,
					  const char *GPIOValue, UICallback SelectorCallback) {
	// Access to current configuration in Board ESP
	// Just only numbers defined in PinManager Object
	// Remove broken and not permitted GPIOs

	uint16_t GPIOPIN_selector =
		ESPUI.addControl(ControlType::Select, GPIOLabel, GPIOValue,
						 ControlColor::Wetasphalt, parentRef, SelectorCallback);
	for (int i = 0; i < ESP_BoardConf::NUM_PINS; i++) {
		if (!ESPAllOnPinManager::getInstance().isPinAttached(i)) {
			ESPUI.addControl(ControlType::Option, getLabelForPin(i), String(i),
							 None, GPIOPIN_selector);
		}
	}
	addElementWithParent(elementToParentMap, GPIOPIN_selector,
						 parentRef); // Add GPIO Selector to parent
}
#endif