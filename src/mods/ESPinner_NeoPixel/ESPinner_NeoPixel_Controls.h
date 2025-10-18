#ifndef _ESPINNER_NEOPIXEL_CONTROLS_H
#define _ESPINNER_NEOPIXEL_CONTROLS_H

#include "../../manager/ESPinner_Manager.h"
#include "./ESPinner_NeoPixel.h"
#include "NeopixelRunner.h"

// NeoPixel control callbacks
void createNeopixel_callback(Control *sender, int type) {}

void Neopixel_action(uint16_t parentRef) {
	auto espinnerNeopixel = std::make_unique<ESPinner_Neopixel>();
	std::vector<uint16_t> childrenIds =
		getChildrenIds(elementToParentMap, parentRef);
	for (uint16_t childControllerId : childrenIds) {
		String espinner_value =
			String(ESPUI.getControl(childControllerId)->value);
		String espinner_label =
			String(ESPUI.getControl(childControllerId)->label);

		if (espinner_label == ESPINNERID_LABEL) {
			espinnerNeopixel->setID(espinner_value);
		}

		if (espinner_label == NEOPIXEL_PIN_SELECTOR_LABEL) {
			espinnerNeopixel->setGPIO(espinner_value.toInt());
			ESP_PinMode NeopixelPinModel = espinnerNeopixel->getPinModeConf();
			ESPAllOnPinManager::getInstance().updateGPIOFromESPUI(
				NeopixelPinModel, childControllerId);
		}

		if (espinner_label == NEOPIXEL_NUMPIXELS_LABEL) {
			espinnerNeopixel->setNumPixels(espinner_value.toInt());
		}
	}
	// Create ESpinner with Configuration
	ESPinner_Manager::getInstance().push(std::move(espinnerNeopixel));
	ESPinner_Manager::getInstance().saveESPinnersInStorage();
}

void Neopixel_updateState_callback(Control *sender, int type) {

	uint16_t parentRef =
		ESPinner_Manager::getInstance().findRefByControllerId(sender->id);

	bool isEN_Ref = (String(sender->label) == NEOPIXEL_SWITCH_EN_LABEL);
	bool isRainbow_Ref =
		(String(sender->label) == NEOPIXEL_SWITCH_RAINBOW_LABEL);
	bool isSpeed_Ref = (String(sender->label) == NEOPIXEL_SPEED_LABEL);
	bool isBrightness_Ref =
		(String(sender->label) == NEOPIXEL_BRIGHTNESS_SLIDER_LABEL);
	bool isColor_Ref = (String(sender->label) == NEOPIXEL_COLOR_LABEL);
	bool isAnimation_Ref =
		(String(sender->label) == NEOPIXEL_ANIMATION_SELECTOR_LABEL);

	ESPinner *espinner = ESPinner_Manager::getInstance().findESPinnerById(
		ESPUI.getControl(parentRef)->value);

	if (espinner && espinner->getType() == ESPinner_Mod::NeoPixel) {
		ESPinner_Neopixel *neopixelPtr =
			static_cast<ESPinner_Neopixel *>(espinner);

		if (isEN_Ref) {
			neopixelPtr->enable(sender->value.toInt() == 1);
			DUMPLN("GPIO: ", neopixelPtr->getGPIO());
			DUMPLN("Num Pixels: ", neopixelPtr->getNumPixels());
			DUMPLN("Rainbow Mode: ", neopixelPtr->getRainbowMode());
			DUMPLN("Speed: ", neopixelPtr->getSpeed());
			DUMPLN("Current Color: ", neopixelPtr->getCurrentColor());
			DUMPLN("Current Animation: ", neopixelPtr->getCurrentAnimation());
			DUMPLN("Loop Animation: ", neopixelPtr->getLoopAnimation());
		}
		if (isBrightness_Ref) {
			neopixelPtr->setBrightness(sender->value.toInt());
		}
		if (isRainbow_Ref) {
			neopixelPtr->setRainbowMode(sender->value.toInt() == 1);
		}
		if (isSpeed_Ref) {
			neopixelPtr->setSpeed(sender->value.toInt());
		}
		if (isColor_Ref) {
			// Convert hex color string (e.g., "#FF0000") to uint32_t
			String hexColor = sender->value;
			if (hexColor.startsWith("#")) {
				hexColor = hexColor.substring(1); // Remove '#' prefix
			}
			uint32_t color = strtoul(hexColor.c_str(), NULL, 16);
			neopixelPtr->setColor(color);
		}
		if (isAnimation_Ref) {
			int animationType = sender->value.toInt();
			DUMPLN("Animation Type Selected: ", animationType);

			// Get interval value from the interval slider (default 50ms if not
			// found)
			uint32_t interval = neopixelPtr->getSpeed();
			// You could search for the interval control here if needed

			switch (animationType) {
			case 0: // VOID
				neopixelPtr->setAnimation(NEOPIXEL_ANIMATION::SOLID, interval);
				break;
			case 1: // BLINK
				neopixelPtr->setAnimation(NEOPIXEL_ANIMATION::BLINK, interval);
				break;
			case 2: // FADE
				neopixelPtr->setAnimation(NEOPIXEL_ANIMATION::FADE, interval);
				break;
			case 3: // INCREMENTAL
				neopixelPtr->setAnimation(NEOPIXEL_ANIMATION::INCREMENTAL,
										  interval);
				break;
			case 4: // DECREMENTAL
				neopixelPtr->setAnimation(NEOPIXEL_ANIMATION::DECREMENTAL,
										  interval);
				break;
			case 5: // RAINBOW
				neopixelPtr->setAnimation(NEOPIXEL_ANIMATION::RAINBOW,
										  interval);
				break;
			case 6: // VOID
				neopixelPtr->stopAnimation();
				break;
			default:
				DUMPLN("Animation type not recognized: ", animationType);
				break;
			}
		}
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

	// Slider BrightnessSwitch
	GUI_Label(NEOPIXEL_Controller_ID, NEOPIXEL_BRIGHTNESS_LABEL,
			  NEOPIXEL_BRIGHTNESS_VALUE, controllerMap, SELECTED_COLOR);
	GUI_Slider(NEOPIXEL_Controller_ID, NEOPIXEL_BRIGHTNESS_SLIDER_LABEL,
			   NEOPIXEL_BRIGHTNESS_SLIDER_VALUE, Neopixel_updateState_callback,
			   controllerMap);

	// Color Picker
	uint16_t text_colour_ID =
		ESPUI.addControl(ControlType::Text, NEOPIXEL_COLOR_LABEL,
						 NEOPIXEL_COLOR_VALUE, ControlColor::Dark,
						 NEOPIXEL_Controller_ID, Neopixel_updateState_callback);
	ESPUI.setInputType(text_colour_ID, "color");
	addElementWithParent(controllerMap, text_colour_ID, NEOPIXEL_Controller_ID);

	// Animation Selector
	GUI_Label(NEOPIXEL_Controller_ID, NEOPIXEL_ANIMATION_SELECT_LABEL,
			  NEOPIXEL_ANIMATION_SELECT_VALUE, controllerMap, SELECTED_COLOR);
	uint16_t animationSelector = ESPUI.addControl(
		ControlType::Select, NEOPIXEL_ANIMATION_SELECTOR_LABEL,
		NEOPIXEL_ANIMATION_SELECTOR_VALUE, ControlColor::Turquoise,
		NEOPIXEL_Controller_ID, Neopixel_updateState_callback);

	// Add animation options
	ESPUI.addControl(ControlType::Option, NEOPIXEL_ANIMATION_VOID_LABEL,
					 NEOPIXEL_ANIMATION_VOID_VALUE, None, animationSelector);
	ESPUI.addControl(ControlType::Option, NEOPIXEL_ANIMATION_BLINK_LABEL,
					 NEOPIXEL_ANIMATION_BLINK_VALUE, None, animationSelector);
	ESPUI.addControl(ControlType::Option, NEOPIXEL_ANIMATION_FADE_LABEL,
					 NEOPIXEL_ANIMATION_FADE_VALUE, None, animationSelector);
	ESPUI.addControl(ControlType::Option, NEOPIXEL_ANIMATION_INCREMENTAL_LABEL,
					 NEOPIXEL_ANIMATION_INCREMENTAL_VALUE, None,
					 animationSelector);
	ESPUI.addControl(ControlType::Option, NEOPIXEL_ANIMATION_DECREMENTAL_LABEL,
					 NEOPIXEL_ANIMATION_DECREMENTAL_VALUE, None,
					 animationSelector);
	ESPUI.addControl(ControlType::Option, NEOPIXEL_ANIMATION_RAINBOW_LABEL,
					 NEOPIXEL_ANIMATION_RAINBOW_VALUE, None, animationSelector);

	addElementWithParent(controllerMap, animationSelector,
						 NEOPIXEL_Controller_ID);

	// Rainbow Switch (kept for compatibility)
	GUI_Label(NEOPIXEL_Controller_ID, NEOPIXEL_INTERVAL_LABEL,
			  NEOPIXEL_INTERVAL_VALUE, controllerMap, SELECTED_COLOR);
	uint16_t mainSlider = GUI_Slider(
		NEOPIXEL_Controller_ID, NEOPIXEL_SPEED_LABEL, NEOPIXEL_SPEED_VALUE,
		Neopixel_updateState_callback, controllerMap);
	ESPUI.addControl(Min, "", NEOPIXEL_INTERVAL_MIN_VALUE, None, mainSlider);
	ESPUI.addControl(Max, "", NEOPIXEL_INTERVAL_MAX_VALUE, None, mainSlider);

	ESPinner_Manager::getInstance().addESPinnerControllerMapping(
		ID_LABEL, NEOPIXEL_Controller_ID);

	ESPinner_Manager::getInstance().addUIRelation(parentRef,
												  NEOPIXEL_Controller_ID);
}

void NeopixelSelector_callback(Control *sender, int type) {
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);

	if (isNumericAndInRange(sender->value, sender->id)) {

		bool isNP_PIN = (String(sender->label) == NEOPIXEL_PIN_SELECTOR_LABEL);
		bool isNP_NumPixels =
			(String(sender->label) == NEOPIXEL_NUMPIXELS_LABEL);

		char *backgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(sender->id, backgroundStyle);
	} else {
		char *backgroundStyle = getBackground(DANGER_COLOR);
		ESPUI.setElementStyle(sender->id, backgroundStyle);
	}
}

void NeopixelNumPixels_callback(Control *sender, int type) {

	if (sender->value.toInt() > 0) {
		bool isNP_NumPixels =
			(String(sender->label) == NEOPIXEL_NUMPIXELS_LABEL);

		char *backgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(sender->id, backgroundStyle);
	} else {
		char *backgroundStyle = getBackground(DANGER_COLOR);
		ESPUI.setElementStyle(sender->id, backgroundStyle);
	}
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

void Neopixel_Selector(uint16_t NP_ptr) {
	// Add ID input field

	GUI_setLabel(NP_ptr, NEOPIXEL_PIN_SELECT_LABEL, NEOPIXEL_PIN_SELECT_VALUE);
	GUI_TextField(NP_ptr, NEOPIXEL_PIN_SELECTOR_LABEL,
				  NEOPIXEL_PIN_SELECTOR_VALUE, NeopixelSelector_callback);
	GUI_setLabel(NP_ptr, NEOPIXEL_NUMPIXELS_SELECT_LABEL,
				 NEOPIXEL_NUMPIXELS_SELECT_VALUE);
	GUI_TextField(NP_ptr, NEOPIXEL_NUMPIXELS_LABEL, NEOPIXEL_NUMPIXELS_VALUE,
				  NeopixelSelector_callback);
}

void saveNeopixel_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		// Create new Neopixel instance with configuration
		uint16_t NP_SelectorRef =
			searchByLabel(parentRef, NEOPIXEL_PIN_SELECTOR_LABEL);
		uint16_t NumPixelsRef =
			searchByLabel(parentRef, NEOPIXEL_NUMPIXELS_LABEL);
		uint16_t NeopixelIDRef = searchByLabel(parentRef, ESPINNERID_LABEL);

		if (NP_SelectorRef != 0 && NumPixelsRef != 0) {
			String NP_GPIOSelector_value =
				ESPUI.getControl(NP_SelectorRef)->value;
			String NumPixels_value = ESPUI.getControl(NumPixelsRef)->value;
			if (isNumericAndInRange(NP_GPIOSelector_value, NP_SelectorRef) &&
				NumPixels_value.toInt() > 0) {

				Neopixel_action(parentRef);
				// ----- Create Controllers ------ //
				uint16_t controller = getParentId(
					ESPinner_Manager::getInstance().getUIRelationIDMap(),
					parentRef);
				if (controller == 0) {
					Neopixel_Controller(ESPUI.getControl(NeopixelIDRef)->value,
										parentRef);
				}

				char *backgroundStyle = getBackground(SELECTED_COLOR);
				ESPUI.setPanelStyle(parentRef, backgroundStyle);

				// Get the NeoPixel instance that was just saved
				String neopixelId = ESPUI.getControl(NeopixelIDRef)->value;
				ESPinner *espinner =
					ESPinner_Manager::getInstance().findESPinnerById(
						neopixelId);

				if (espinner && espinner->getType() == ESPinner_Mod::NeoPixel) {
					ESPinner_Neopixel *neopixelPtr =
						static_cast<ESPinner_Neopixel *>(espinner);
					// Register the NeoPixel with the runner using its ID
					bool registered =
						neopixelPtr->registerRunner(neopixelPtr->getID());
					if (registered) {
						DUMPSLN(
							"NeoPixel registrado exitosamente en el runner");
					} else {
						DUMPSLN("Error: No se pudo registrar el NeoPixel en el "
								"runner");
					}
				}

			} else {
				char *backgroundStyle = getBackground(PENDING_COLOR);
				ESPUI.setPanelStyle(parentRef, backgroundStyle);
			}
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
			String espinner_label =
				String(ESPUI.getControl(childControllerId)->label);

			if (espinner_label == NEOPIXEL_PIN_SELECTOR_LABEL) {
				detachRemovedPIN(NEOPIXEL_PIN_SELECTOR_LABEL, espinner_label,
								 espinner_value);
			}
		}
		removeElement_callback(sender, type);
	}
}

void Neopixel_UI(uint16_t NP_ptr) {
	// Add configuration interface
	Neopixel_Selector(NP_ptr);
	GUIButtons_Elements(NP_ptr, NEOPIXEL_SAVE_LABEL, NEOPIXEL_SAVE_VALUE,
						NEOPIXEL_REMOVE_LABEL, NEOPIXEL_REMOVE_VALUE,
						saveNeopixel_callback, removeElement_callback);
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

	GUI_setLabel(Neopixel_PIN_selector, NEOPIXEL_PIN_SELECT_LABEL,
				 NEOPIXEL_PIN_SELECT_VALUE, SELECTED_COLOR);
	String gpioNP = String(ESPinner_Neopixel::getGPIO());
	uint16_t gpioNP_ref =
		GUI_TextField(Neopixel_PIN_selector, NEOPIXEL_PIN_SELECTOR_LABEL,
					  gpioNP.c_str(), NeopixelSelector_callback);

	GUI_setLabel(Neopixel_PIN_selector, NEOPIXEL_NUMPIXELS_SELECT_LABEL,
				 NEOPIXEL_NUMPIXELS_SELECT_VALUE, SELECTED_COLOR);
	String numPixelsNP = String(ESPinner_Neopixel::getNumPixels());
	uint16_t numPixelsNP_ref =
		GUI_TextField(Neopixel_PIN_selector, NEOPIXEL_NUMPIXELS_LABEL,
					  numPixelsNP.c_str(), NeopixelNumPixels_callback);

	GUIButtons_Elements(Neopixel_PIN_selector, NEOPIXEL_SAVE_LABEL,
						NEOPIXEL_SAVE_VALUE, NEOPIXEL_REMOVE_LABEL,
						NEOPIXEL_REMOVE_VALUE, saveNeopixel_callback,
						removeNeopixel_callback);

	ESPAllOnPinManager::getInstance().setPinControlRelation(
		ESPinner_Neopixel::getGPIO(), gpioNP_ref);

	ESP_PinMode NP_GPIO_Model = ESP_PinMode(ESPinner_Neopixel::getGPIO(),
											OutputPin(), PinType::BusDigital);
	ESPAllOnPinManager::getInstance().attach(NP_GPIO_Model);

	// ------- Create Controllers ----------- //
	Neopixel_Controller(ESPinner_Neopixel::getID(), Neopixel_PIN_selector);
}

#endif