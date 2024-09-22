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
	for (const auto &pair : ESPAllOnPinManager::getInstance().gpioLabels) {
		// TODO Pins attached not included, neither broken not used with Wifi
		// or other issue.
		if (!ESPAllOnPinManager::getInstance().isPinAttached(pair.first)) {
			ESPUI.addControl(ControlType::Option,
							 getLabelFromPinManager(pair.first),
							 String(pair.first), None, GPIOPIN_selector);
		}
	}
	addElementWithParent(elementToParentMap, GPIOPIN_selector,
						 parentRef); // Add GPIO Selector to parent
}
#endif