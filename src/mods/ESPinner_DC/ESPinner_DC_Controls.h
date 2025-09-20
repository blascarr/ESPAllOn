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

void DC_action(uint16_t parentRef) {
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
			ESP_PinMode pinModelA = espinnerDC->getPinModeConf(DCPin::PinA);
			ESPAllOnPinManager::getInstance().updateGPIOFromESPUI(
				pinModelA, childControllerId);
		}
		if (espinner_label == DC_PINB_SELECTOR_LABEL) {

			espinnerDC->setGPIOB(espinner_value.toInt());
			ESP_PinMode pinModelB = espinnerDC->getPinModeConf(DCPin::PinB);
			ESPAllOnPinManager::getInstance().updateGPIOFromESPUI(
				pinModelB, childControllerId);
		}
	}
	// Create ESpinner with Configuration
	ESPinner_Manager::getInstance().push(std::move(espinnerDC));
	ESPinner_Manager::getInstance().saveESPinnersInStorage();
}

void DC_Selector(uint16_t PIN_ptr) {
	GUI_setLabel(PIN_ptr, DC_PINA_SELECT_LABEL, DC_PINA_SELECT_VALUE);
	GUI_GPIOSelector(PIN_ptr, DC_PINA_SELECTOR_LABEL, DC_PINA_SELECTOR_VALUE,
					 DCSelector_callback);

	GUI_setLabel(PIN_ptr, DC_PINB_SELECT_LABEL, DC_PINB_SELECT_VALUE);
	GUI_GPIOSelector(PIN_ptr, DC_PINB_SELECTOR_LABEL, DC_PINB_SELECTOR_VALUE,
					 DCSelector_callback);
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
			if (espinner_label == DC_PINA_SELECTOR_LABEL) {
				detachRemovedPIN(DC_PINA_SELECTOR_LABEL, espinner_label,
								 espinner_value);
			}
			if (espinner_label == DC_PINB_SELECTOR_LABEL) {
				detachRemovedPIN(DC_PINB_SELECTOR_LABEL, espinner_label,
								 espinner_value);
			}
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

void DCSelector_callback(Control *sender, int type) {
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (isNumericAndInRange(sender->value, sender->id)) {

		bool isPinA = (String(sender->label) == DC_PINA_SELECTOR_LABEL);
		bool isPinB = (String(sender->label) == DC_PINB_SELECTOR_LABEL);

		if (isPinA) {
			uint16_t selectLabelRefA =
				searchByLabel(parentRef, DC_PINA_SELECT_LABEL);
			if (selectLabelRefA != 0) {
				ESPUI.removeControl(selectLabelRefA);
				removeValueFromMap(elementToParentMap, selectLabelRefA);
			}
		}

		if (isPinB) {
			uint16_t selectLabelRefB =
				searchByLabel(parentRef, DC_PINB_SELECT_LABEL);
			if (selectLabelRefB != 0) {
				ESPUI.removeControl(selectLabelRefB);
				removeValueFromMap(elementToParentMap, selectLabelRefB);
			}
		}

		char *backgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(sender->id, backgroundStyle);

	} else {
		char *backgroundStyle = getBackground(DANGER_COLOR);
		ESPUI.setElementStyle(sender->id, backgroundStyle);
	}
}

void moveDC(uint8_t pinA, uint8_t pinB, int pwmValue, bool CW) {
	uint8_t pwmVA = CW ? map(pwmValue, 0, 100, 0, 255) : 0;
	uint8_t pwmVB = CW ? 0 : map(pwmValue, 0, 100, 0, 255);

	analogWrite(pinA, pwmVA);
	analogWrite(pinB, pwmVB);
}

void updateDCMotorState(uint16_t parentRef) {
	uint16_t PINASelectorRef = searchByLabel(parentRef, DC_PINA_SELECTOR_LABEL);
	uint16_t PINBSelectorRef = searchByLabel(parentRef, DC_PINB_SELECTOR_LABEL);

	uint8_t associatedPinA =
		ESPAllOnPinManager::getInstance().getCurrentReference(PINASelectorRef);
	uint8_t associatedPinB =
		ESPAllOnPinManager::getInstance().getCurrentReference(PINBSelectorRef);

	uint16_t RUNSwitchRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, DC_SWITCH_RUN_LABEL);

	bool runValue = ESPUI.getControl(RUNSwitchRef)->value.toInt() == 1;
	if (runValue == 0) {

		analogWrite(associatedPinA, 0);
		analogWrite(associatedPinB, 0);
	} else {
		uint16_t VELSwitchRef = searchInMapByLabel(
			ESPinner_Manager::getInstance().getControllerMap(), parentRef,
			DC_SLIDER_VEL_LABEL);

		uint16_t DIRSwitchRef = searchInMapByLabel(
			ESPinner_Manager::getInstance().getControllerMap(), parentRef,
			DC_SWITCH_DIR_LABEL);

		int pwmValue = ESPUI.getControl(VELSwitchRef)->value.toInt();
		bool DIRValue = ESPUI.getControl(DIRSwitchRef)->value.toInt() == 1;

		moveDC(associatedPinA, associatedPinB, pwmValue, DIRValue);
	}
}

void DC_VEL_Slider_callback(Control *sender, int type) {
	uint16_t parentRef =
		ESPinner_Manager::getInstance().findRefByControllerId(sender->id);
	updateDCMotorState(parentRef);
}

void DC_DIR_Switcher_callback(Control *sender, int type) {
	uint16_t parentRef =
		ESPinner_Manager::getInstance().findRefByControllerId(sender->id);
	updateDCMotorState(parentRef);
}

void DC_RUN_Switcher_callback(Control *sender, int type) {
	uint16_t parentRef =
		ESPinner_Manager::getInstance().findRefByControllerId(sender->id);
	updateDCMotorState(parentRef);
}

void DC_Controller(String ID_LABEL, uint16_t parentRef) {

	uint16_t controllerTabRef = getTab(TabType::ControllerTab);
	uint16_t DCPIN_ID = ESPUI.addControl(
		ControlType::Text, DC_ID_LABEL, ID_LABEL.c_str(),
		ControlColor::Wetasphalt, controllerTabRef, debugCallback);
	ESPUI.getControl(DCPIN_ID)->enabled = false;

	uint16_t DC_DIR_Switch = ESPUI.addControl(
		ControlType::Switcher, DC_SWITCH_DIR_LABEL, DC_SWITCH_DIR_VALUE,
		ControlColor::Wetasphalt, DCPIN_ID, DC_DIR_Switcher_callback);

	uint16_t DC_RUN_Switch = ESPUI.addControl(
		ControlType::Switcher, DC_SWITCH_RUN_LABEL, DC_SWITCH_RUN_VALUE,
		ControlColor::Wetasphalt, DCPIN_ID, DC_RUN_Switcher_callback);

	uint16_t DC_VEL_Slider = ESPUI.addControl(
		ControlType::Slider, DC_SLIDER_VEL_LABEL, DC_SLIDER_VEL_VALUE,
		ControlColor::Wetasphalt, DCPIN_ID, DC_VEL_Slider_callback);

	ESPinner_Manager::getInstance().addControllerRelation(DC_DIR_Switch,
														  parentRef);
	ESPinner_Manager::getInstance().addControllerRelation(DC_RUN_Switch,
														  parentRef);
	ESPinner_Manager::getInstance().addControllerRelation(DC_VEL_Slider,
														  parentRef);

	ESPinner_Manager::getInstance().addUIRelation(parentRef, DCPIN_ID);
}

void saveDC_callback(Control *sender, int type) {

	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		uint16_t DCSelectorRefA =
			searchByLabel(parentRef, DC_PINA_SELECTOR_LABEL);
		uint16_t DCSelectorRefB =
			searchByLabel(parentRef, DC_PINB_SELECTOR_LABEL);
		uint16_t DCIDRef = searchByLabel(parentRef, ESPINNERID_LABEL);
		if (DCSelectorRefA != 0 && DCSelectorRefB != 0) {
			String DCSelector_valueA = ESPUI.getControl(DCSelectorRefA)->value;
			String DCSelector_valueB = ESPUI.getControl(DCSelectorRefB)->value;

			if (isNumericAndInRange(DCSelector_valueA, DCSelectorRefA) &&
				isNumericAndInRange(DCSelector_valueB, DCSelectorRefB)) {

				DC_action(parentRef);

				// ----- Create Controllers ------ //
				uint16_t controller = getParentId(
					ESPinner_Manager::getInstance().getUIRelationIDMap(),
					parentRef);
				if (controller == 0) {
					DC_Controller(ESPUI.getControl(DCIDRef)->value, parentRef);
				}

				char *backgroundStyle = getBackground(SELECTED_COLOR);
				ESPUI.setPanelStyle(parentRef, backgroundStyle);
			} else {
				char *backgroundStyle = getBackground(PENDING_COLOR);
				ESPUI.setPanelStyle(parentRef, backgroundStyle);
			}
		}
	}
}

void ESPinner_DC::implement() {
	uint16_t parentRef = getTab(TabType::BasicTab);

	uint16_t DCPIN_selector = ESPUI.addControl(
		ControlType::Text, ESPINNERID_LABEL, ESPinner_DC::getID(),
		ControlColor::Wetasphalt, parentRef, DCSelector_callback);
	ESPUI.getControl(DCPIN_selector)->enabled = false;
	addElementWithParent(elementToParentMap, DCPIN_selector, DCPIN_selector);

	String gpioA = String(ESPinner_DC::getGPIOA());
	uint16_t gpioA_ref =
		GUI_GPIOSelector(DCPIN_selector, DC_PINA_SELECTOR_LABEL, gpioA.c_str(),
						 DCSelector_callback);

	String gpioB = String(ESPinner_DC::getGPIOB());
	uint16_t gpioB_ref =
		GUI_GPIOSelector(DCPIN_selector, DC_PINB_SELECTOR_LABEL, gpioB.c_str(),
						 DCSelector_callback);

	GUIButtons_Elements(DCPIN_selector, DC_SAVE_LABEL, DC_SAVE_VALUE,
						REMOVEESPINNER_LABEL, REMOVEESPINNER_VALUE,
						saveDC_callback, removeDC_callback);

	ESPAllOnPinManager::getInstance().setPinControlRelation(
		ESPinner_DC::getGPIOA(), gpioA_ref);
	ESPAllOnPinManager::getInstance().setPinControlRelation(
		ESPinner_DC::getGPIOB(), gpioB_ref);

	ESP_PinMode pinModelA =
		ESP_PinMode(ESPinner_DC::getGPIOA(), OutputPin(), PinType::BusPWM);
	ESPAllOnPinManager::getInstance().attach(pinModelA);
	ESP_PinMode pinModelB =
		ESP_PinMode(ESPinner_DC::getGPIOB(), OutputPin(), PinType::BusPWM);
	ESPAllOnPinManager::getInstance().attach(pinModelB);

	// ------- Create Controllers ----------- //
	DC_Controller(ESPinner_DC::getID(), DCPIN_selector);
}

#endif