#ifndef _ESPINNER_DC_CONTROLS_H
#define _ESPINNER_DC_CONTROLS_H

#include "./ESPinner_DC.h"

#include "../../manager/ESPinner_Manager.h"

void createDC_callback(Control *sender, int type) {}
/*
 *	DC_ModeSelector
 *	DC_Pin1
 *	DC_Save
 *	DC_Remove
 */

void DC_save_action(uint16_t parentRef, uint16_t GPIOSelectorRef) {
	auto espinnerDC = std::make_unique<ESPinner_DC>();
	std::vector<uint16_t> childrenIds =
		getChildrenIds(elementToParentMap, parentRef);
	for (uint16_t childControllerId : childrenIds) {
		String espinner_value =
			String(ESPUI.getControl(childControllerId)->value);
		String espinner_label =
			String(ESPUI.getControl(childControllerId)->label);

		if (espinner_label == ESPINNERID_LABEL) {
			espinnerDC->setID(espinner_value);
		}

		if (espinner_label == DC_PINA_SELECTOR_LABEL) {
			espinnerDC->setGPIOA(espinner_value.toInt());
		}
		if (espinner_label == DC_PINB_SELECTOR_LABEL) {
			espinnerDC->setGPIOB(espinner_value.toInt());
		}
	}
	// Create ESpinner with Configuration
	ESP_PinMode pinModelA = espinnerDC->getPinModeConf(DCPin::PinA);
	ESPAllOnPinManager::getInstance().updateGPIOFromESPUI(pinModelA,
														  GPIOSelectorRef);
	ESP_PinMode pinModelB = espinnerDC->getPinModeConf(DCPin::PinB);
	ESPAllOnPinManager::getInstance().updateGPIOFromESPUI(pinModelB,
														  GPIOSelectorRef);
	ESPinner_Manager::getInstance().push(std::move(espinnerDC));
	ESPinner_Manager::getInstance().saveESPinnersInStorage();
}

void saveDC_callback(Control *sender, int type) {
	// Save Button --> Save ESPINNER
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		uint16_t DCSelectorRefA =
			searchByLabel(parentRef, DC_PINA_SELECTOR_LABEL);
		uint16_t DCSelectorRefB =
			searchByLabel(parentRef, DC_PINB_SELECTOR_LABEL);
		DUMPLN("IS THERE DC A PIN ", DCSelectorRefA);
		DUMPLN("IS THERE DC B PIN ", DCSelectorRefB);
		if (DCSelectorRefA != 0 && DCSelectorRefB != 0) {
			String DCSelector_valueA = ESPUI.getControl(DCSelectorRefA)->value;
			String DCSelector_valueB = ESPUI.getControl(DCSelectorRefA)->value;

			// If Text Input --> Check if number
			if (isNumericAndInRange(DCSelector_valueA, DCSelectorRefA) &&
				isNumericAndInRange(DCSelector_valueB, DCSelectorRefB)) {

				saveButtonGPIOCheck(parentRef, DC_PINA_SELECTOR_LABEL,
									DC_save_action);
				saveButtonGPIOCheck(parentRef, DC_PINB_SELECTOR_LABEL,
									DC_save_action);
				// saveButtonCheck(parentRef, DC_PININPUT_LABEL, DC_SAVE_LABEL);
				char *backgroundStyle = getBackground(SELECTED_COLOR);
				ESPUI.setPanelStyle(parentRef, backgroundStyle);
			} else {
				char *backgroundStyle = getBackground(PENDING_COLOR);
				ESPUI.setPanelStyle(parentRef, backgroundStyle);
			}
		}
	}
}

void DC_Selector(uint16_t PIN_ptr) {
	GUI_GPIOSetLabel(PIN_ptr, DC_PINA_SELECT_LABEL, DC_PINA_SELECT_VALUE);
	GUI_GPIOSelector(PIN_ptr, DC_PINA_SELECTOR_LABEL, DC_PINA_SELECTOR_VALUE,
					 GPIOSelector_callback);

	GUI_GPIOSetLabel(PIN_ptr, DC_PINB_SELECT_LABEL, DC_PINB_SELECT_VALUE);
	GUI_GPIOSelector(PIN_ptr, DC_PINB_SELECTOR_LABEL, DC_PINB_SELECTOR_VALUE,
					 GPIOSelector_callback);
}

void removeDC_callback(Control *sender, int type) {
	if (type == B_UP) {
		debugCallback(sender, type);
		uint16_t parentRef = getParentId(elementToParentMap, sender->id);

		std::vector<uint16_t> childrenIds =
			getChildrenIds(elementToParentMap, parentRef);
		for (uint16_t childControllerId : childrenIds) {
			String espinner_value =
				String(ESPUI.getControl(childControllerId)->value);
			String espinner_label =
				String(ESPUI.getControl(childControllerId)->label);

			// Review which Pin is disconnected in order to detach from
			// ESPinnerManager
			detachRemovedPIN(DC_PINA_SELECTOR_LABEL, espinner_label,
							 espinner_value);
			detachRemovedPIN(DC_PINB_SELECTOR_LABEL, espinner_label,
							 espinner_value);
		}
		removeElement_callback(sender, type);
	}
}

void DC_UI(uint16_t DC_ptr) {
	DC_Selector(DC_ptr);
	GUIButtons_Elements(DC_ptr, DC_SAVE_LABEL, DC_SAVE_VALUE, DC_REMOVE_LABEL,
						DC_REMOVE_VALUE, saveDC_callback,
						removeElement_callback);
}

void ESPinner_DC::implement() {
	uint16_t parentRef = getTab(TabType::BasicTab);

	uint16_t DCPIN_selector = ESPUI.addControl(
		ControlType::Text, ESPINNERID_LABEL, ESPinner_DC::getID(),
		ControlColor::Wetasphalt, parentRef, GPIOSelector_callback);
	ESPUI.getControl(DCPIN_selector)->enabled = false;
	addElementWithParent(elementToParentMap, DCPIN_selector, DCPIN_selector);

	String gpioA = String(ESPinner_DC::getGPIOA());
	uint16_t gpioA_ref =
		GUI_GPIOSelector(DCPIN_selector, DC_PINA_SELECTOR_LABEL, gpioA.c_str(),
						 GPIOSelector_callback);

	String gpioB = String(ESPinner_DC::getGPIOA());
	uint16_t gpioB_ref =
		GUI_GPIOSelector(DCPIN_selector, DC_PINB_SELECTOR_LABEL, gpioB.c_str(),
						 GPIOSelector_callback);

	GUIButtons_Elements(DCPIN_selector, GPIO_SAVE_LABEL, GPIO_SAVE_VALUE,
						REMOVEESPINNER_LABEL, REMOVEESPINNER_VALUE,
						saveGPIO_callback, removeDC_callback);

	ESPAllOnPinManager::getInstance().setPinControlRelation(
		ESPinner_DC::getGPIOA(), gpioA_ref);
	ESPAllOnPinManager::getInstance().setPinControlRelation(
		ESPinner_DC::getGPIOB(), gpioB_ref);
	ESPAllOnPinManager::getInstance().attach(ESPinner_DC::getGPIOA());
	ESPAllOnPinManager::getInstance().attach(ESPinner_DC::getGPIOB());
}

#endif