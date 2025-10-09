#ifndef _ESPALLONGUI_H
#define _ESPALLONGUI_H
#include "../../utils.h"
#include "../ESPAllOnPinManager.h"
#include <Arduino.h>
#include <ESPUI.h>

template <typename MapType>
uint16_t GUI_Factory(ControlType type, uint16_t parentRef, const char *label,
					 const char *value, UICallback selectorCallback,
					 MapType &targetMap,
					 ControlColor color = ControlColor::Wetasphalt) {

	uint16_t controlId = ESPUI.addControl(type, label, value, color, parentRef,
										  selectorCallback);
	addElementWithParent(targetMap, controlId, parentRef);
	return controlId;
}

template <typename MapType>
uint16_t GUI_Text(uint16_t parentRef, const char *label, const char *value,
				  UICallback selectorCallback, MapType &targetMap,
				  ControlColor color = ControlColor::Wetasphalt) {
	return GUI_Factory(ControlType::Text, parentRef, label, value,
					   selectorCallback, targetMap, color);
}

template <typename MapType>
uint16_t GUI_Label(uint16_t parentRef, const char *label, const char *value,
				   MapType &targetMap, const char *color = SELECTED_COLOR,
				   UICallback labelCallback = nullptr) {
	uint16_t label_selector =
		GUI_Factory(ControlType::Label, parentRef, label, value, labelCallback,
					targetMap, ControlColor::Carrot);

	char *backgroundStyle = getBackground(color);
	ESPUI.setElementStyle(label_selector, backgroundStyle);

	return label_selector;
}

template <typename MapType>
uint16_t GUI_Switcher(uint16_t parentRef, const char *label, const char *value,
					  UICallback selectorCallback, MapType &targetMap,
					  ControlColor color = ControlColor::Wetasphalt) {
	return GUI_Factory(ControlType::Switcher, parentRef, label, value,
					   selectorCallback, targetMap, color);
}

template <typename MapType>
uint16_t GUI_Button(uint16_t parentRef, const char *label, const char *value,
					UICallback selectorCallback, MapType &targetMap,
					const char *color = SELECTED_COLOR) {
	uint16_t button_selector =
		GUI_Factory(ControlType::Button, parentRef, label, value,
					selectorCallback, targetMap, ControlColor::Wetasphalt);
	char *backgroundStyle = getBackground(color);
	ESPUI.setElementStyle(button_selector, backgroundStyle);
	return button_selector;
}

template <typename MapType>
uint16_t GUI_Slider(uint16_t parentRef, const char *label, const char *value,
					UICallback selectorCallback, MapType &targetMap,
					ControlColor color = ControlColor::Wetasphalt) {
	return GUI_Factory(ControlType::Slider, parentRef, label, value,
					   selectorCallback, targetMap, color);
}

template <typename MapType>
uint16_t GUI_PadWithCenter(uint16_t parentRef, const char *label,
						   const char *value, UICallback selectorCallback,
						   MapType &targetMap,
						   ControlColor color = ControlColor::Wetasphalt) {
	return GUI_Factory(ControlType::PadWithCenter, parentRef, label, value,
					   selectorCallback, targetMap, color);
}

uint16_t GUI_TextField(uint16_t parentRef, const char *label, const char *value,
					   UICallback SelectorCallback) {

	return GUI_Text(parentRef, label, value, SelectorCallback,
					elementToParentMap);
}

void GUI_setLabel(uint16_t parentRef, const char *label, const char *value,
				  const char *color = SELECTED_COLOR) {
	GUI_Label(parentRef, label, value, elementToParentMap, color);
}

#endif