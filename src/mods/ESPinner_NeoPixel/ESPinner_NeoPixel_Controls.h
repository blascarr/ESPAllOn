#ifndef _ESPINNER_NEOPIXEL_CONTROLS_H
#define _ESPINNER_NEOPIXEL_CONTROLS_H

#include "../../manager/ESPinner_Manager.h"
#include "./ESPinner_NeoPixel.h"
#include "NeopixelRunner.h"

// NeoPixel control callbacks
void createNeopixel_callback(Control *sender, int type) {}

void saveNeopixel_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		// Create new Neopixel instance with configuration
		auto neopixel = std::make_unique<ESPinner_Neopixel>();

		// Get all child controls to extract configuration
		std::vector<uint16_t> childrenIds =
			getChildrenIds(elementToParentMap, parentRef);

		String neopixelId = "";
		int gpioPin = 0;
		int numPixels = 8;

		for (uint16_t childControllerId : childrenIds) {
			String controlValue =
				String(ESPUI.getControl(childControllerId)->value);
			String controlLabel =
				String(ESPUI.getControl(childControllerId)->label);

			if (controlLabel == NEOPIXEL_ID_LABEL) {
				neopixelId = controlValue;
			} else if (controlLabel == NEOPIXEL_PINSELECTOR_LABEL) {
				gpioPin = controlValue.toInt();
			} else if (controlLabel == NEOPIXEL_NUMPIXELS_LABEL) {
				numPixels = controlValue.toInt();
			}
		}

		// Validate configuration
		if (neopixelId.length() > 0 && gpioPin > 0 && numPixels > 0) {
			// Set configuration
			neopixel->setID(neopixelId);

			// Configure the NeoPixel strip
			neopixel->configureStrip(gpioPin, numPixels);

			// Register with manager
			ESPinner_Manager::getInstance().push(std::move(neopixel));
			ESPinner_Manager::getInstance().saveESPinnersInStorage();

			// Show success message
			ESPUI.setElementStyle(sender->id, "background-color: #2ecc71;");
		} else {
			// Show error message
			ESPUI.setElementStyle(sender->id, "background-color: #e74c3c;");
		}
	}
}
void removeNeopixel_callback(Control *sender, int type) {
	if (type == B_UP) {
		uint16_t parentRef = getParentId(elementToParentMap, sender->id);
		std::vector<uint16_t> childrenIds =
			getChildrenIds(elementToParentMap, parentRef);
		for (uint16_t childControllerId : childrenIds) {
			String espinner_value =
				String(ESPUI.getControl(childControllerId)->value);
		}
	}
}

void NeopixelSelector_callback(Control *sender, int type) {
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	String neopixelId = ESPUI.getControl(parentRef)->value;
}

void Neopixel_color_callback(Control *sender, int type) {
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	String neopixelId = ESPUI.getControl(parentRef)->value;

	auto neopixel = NeopixelRunner::getInstance().getRunnable(neopixelId);
	if (neopixel) {
		// Cast to ESPinner_Neopixel to access setColor method
		ESPinner_Neopixel *neopixelPtr =
			static_cast<ESPinner_Neopixel *>(neopixel.get());
		// Convert hex color string to uint32_t
		uint32_t color = strtoul(sender->value.c_str(), NULL, 16);
		neopixelPtr->setColor(color);
	}
}

void Neopixel_rainbow_callback(Control *sender, int type) {
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	String neopixelId = ESPUI.getControl(parentRef)->value;

	auto neopixel = NeopixelRunner::getInstance().getRunnable(neopixelId);
	if (neopixel) {
		// Cast to ESPinner_Neopixel to access setRainbowMode method
		ESPinner_Neopixel *neopixelPtr =
			static_cast<ESPinner_Neopixel *>(neopixel.get());
		neopixelPtr->setRainbowMode(sender->value.toInt() == 1);
	}
}

void Neopixel_updateState_callback(Control *sender, int type) {
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	String neopixelId = ESPUI.getControl(parentRef)->value;

	auto neopixel = NeopixelRunner::getInstance().getRunnable(neopixelId);
	if (neopixel) {
		ESPinner_Neopixel *neopixelPtr =
			static_cast<ESPinner_Neopixel *>(neopixel.get());
		neopixelPtr->setPower(sender->value.toInt() == 1);
		neopixelPtr->setRainbowMode(sender->value.toInt() == 1);
	}
}

void Neopixel_Controller(String ID_LABEL, uint16_t parentRef) {
	uint16_t controllerTabRef = getTab(TabType::ControllerTab);

	uint16_t NEOPIXEL_Controller_ID = ESPUI.addControl(
		ControlType::Text, NEOPIXEL_ID_LABEL, ID_LABEL.c_str(),
		ControlColor::Wetasphalt, controllerTabRef, debugCallback);
	ESPUI.getControl(NEOPIXEL_Controller_ID)->enabled = false;

	std::map<uint16_t, uint16_t> &controllerMap =
		ESPinner_Manager::getInstance().getControllerMap();
	// Enable ON/OFF Switch with Label
	GUI_Label(NEOPIXEL_Controller_ID, NEOPIXEL_EN_LABEL, NEOPIXEL_EN_VALUE,
			  controllerMap, SELECTED_COLOR);

	GUI_Switcher(NEOPIXEL_Controller_ID, NEOPIXEL_SWITCH_EN_LABEL,
				 NEOPIXEL_SWITCH_EN_VALUE, Neopixel_updateState_callback,
				 controllerMap);

	// Rainbow Switch
	GUI_Label(NEOPIXEL_Controller_ID, NEOPIXEL_RAINBOW_LABEL,
			  NEOPIXEL_RAINBOW_VALUE, controllerMap, SELECTED_COLOR);

	GUI_Switcher(NEOPIXEL_Controller_ID, NEOPIXEL_SWITCH_RAINBOW_LABEL,
				 NEOPIXEL_SWITCH_RAINBOW_VALUE, Neopixel_rainbow_callback,
				 controllerMap);

	GUI_Slider(NEOPIXEL_Controller_ID, NEOPIXEL_SPEED_LABEL,
			   NEOPIXEL_SPEED_VALUE, Neopixel_rainbow_callback, controllerMap);

	ESPinner_Manager::getInstance().addESPinnerControllerMapping(
		ID_LABEL, NEOPIXEL_Controller_ID);

	ESPinner_Manager::getInstance().addUIRelation(parentRef,
												  NEOPIXEL_Controller_ID);
}

void Neopixel_Selector(uint16_t NP_ptr) {
	// Add ID input field
	/*
	uint16_t idControl = ESPUI.addControl(ControlType::Text, NEOPIXEL_ID_LABEL,
										  NEOPIXEL_ID_VALUE, ControlColor::Dark,
										  NP_ptr, Neopixel_ID_callback);
	addElementWithParent(elementToParentMap, idControl, NP_ptr);

	// Add GPIO pin selector
	uint16_t gpioControl =
		ESPUI.addControl(ControlType::Text, NEOPIXEL_PINSELECTOR_LABEL,
						 NEOPIXEL_PINSELECTOR_VALUE, ControlColor::Dark, NP_ptr,
						 Neopixel_GPIOSelector_callback);
	addElementWithParent(elementToParentMap, gpioControl, NP_ptr);

	// Add number of pixels input
	uint16_t pixelsControl = ESPUI.addControl(
		ControlType::Text, NEOPIXEL_NUMPIXELS_LABEL, NEOPIXEL_NUMPIXELS_VALUE,
		ControlColor::Dark, NP_ptr, Neopixel_NumPixels_callback);
	addElementWithParent(elementToParentMap, pixelsControl, NP_ptr);
	*/
}

void Neopixel_UI(uint16_t NP_ptr) {
	// Add configuration interface
	/*
	Neopixel_Selector(NP_ptr);

	// Add NeoPixel controls
	uint16_t colorControl = ESPUI.addControl(
		ControlType::Text, NEOPIXEL_COLOR_LABEL, NEOPIXEL_COLOR_VALUE,
		ControlColor::Dark, NP_ptr, Neopixel_color_callback);
	ESPUI.setInputType(colorControl, "color");

	ESPUI.switcher(NEOPIXEL_POWER_LABEL, Neopixel_power_callback,
				   ControlColor::Dark, false);
	ESPUI.switcher(NEOPIXEL_RAINBOW_LABEL, Neopixel_rainbow_callback,
				   ControlColor::Dark, false);
	ESPUI.slider(NEOPIXEL_RAINBOW_SPEED_LABEL, Neopixel_rainbowSpeed_callback,
				 ControlColor::Dark, 50);

	GUIButtons_Elements(NP_ptr, NEOPIXEL_SAVE_LABEL, NEOPIXEL_SAVE_VALUE,
						NEOPIXEL_REMOVE_LABEL, NEOPIXEL_REMOVE_VALUE,
						saveNeopixel_callback, removeElement_callback);
						*/
}

void ESPinner_Neopixel::implement() {
	DUMPSLN("Implementacion configuración de NEOPIXEL...");

	uint16_t parentRef = getTab(TabType::BasicTab);

	uint16_t Neopixel_PIN_selector = ESPUI.addControl(
		ControlType::Text, ESPINNERID_LABEL, ESPinner_Neopixel::getID(),
		ControlColor::Wetasphalt, parentRef, NeopixelSelector_callback);

	ESPUI.getControl(Neopixel_PIN_selector)->enabled = false;
	addElementWithParent(elementToParentMap, Neopixel_PIN_selector,
						 Neopixel_PIN_selector);

	GUI_setLabel(Neopixel_PIN_selector, NEOPIXEL_PINSELECTOR_LABEL,
				 NEOPIXEL_PINSELECTOR_VALUE, SELECTED_COLOR);
	String gpioNP = String(ESPinner_Neopixel::getGPIO());
	uint16_t gpioNP_ref =
		GUI_TextField(Neopixel_PIN_selector, NEOPIXEL_PINSELECTOR_LABEL,
					  gpioNP.c_str(), NeopixelSelector_callback);

	GUI_setLabel(Neopixel_PIN_selector, NEOPIXEL_NUMPIXELS_LABEL,
				 NEOPIXEL_NUMPIXELS_VALUE, SELECTED_COLOR);
	String numPixelsNP = String(ESPinner_Neopixel::getNumPixels());
	uint16_t numPixelsNP_ref =
		GUI_TextField(Neopixel_PIN_selector, NEOPIXEL_PINSELECTOR_LABEL,
					  numPixelsNP.c_str(), NeopixelSelector_callback);

	ESPAllOnPinManager::getInstance().setPinControlRelation(
		ESPinner_Neopixel::getGPIO(), gpioNP_ref);

	GUIButtons_Elements(Neopixel_PIN_selector, NEOPIXEL_SAVE_LABEL,
						NEOPIXEL_SAVE_VALUE, NEOPIXEL_REMOVE_LABEL,
						NEOPIXEL_REMOVE_VALUE, saveNeopixel_callback,
						removeNeopixel_callback);

	// Register with the NeopixelRunner
	// registerRunner(this->getID());
	// ------- Create Controllers ----------- //
	Neopixel_Controller(ESPinner_Neopixel::getID(), Neopixel_PIN_selector);
}

#endif