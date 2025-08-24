#ifndef _ESPINNER_STEPPER_CONTROLS_H
#define _ESPINNER_STEPPER_CONTROLS_H

#include "./ESPinner_Stepper.h"

#include "../../manager/ESPinner_Manager.h"

void createStepper_callback(Control *sender, int type) {}

void Stepper_action(uint16_t parentRef) {
	auto espinnerStepper = std::make_unique<ESPinner_Stepper>();
	std::vector<uint16_t> childrenIds =
		getChildrenIds(elementToParentMap, parentRef);
	for (uint16_t childControllerId : childrenIds) {
		String espinner_value =
			String(ESPUI.getControl(childControllerId)->value);
		String espinner_label =
			String(ESPUI.getControl(childControllerId)->label);
		if (espinner_label == ESPINNERID_LABEL) {
			espinnerStepper->setID(espinner_value);
		}

		if (espinner_label == STEPPER_DIR_SELECTOR_LABEL) {
			espinnerStepper->setDIR(espinner_value.toInt());
			ESP_PinMode pinModelDIR =
				espinnerStepper->getPinModeConf(StepperPin::DIR);
			ESPAllOnPinManager::getInstance().updateGPIOFromESPUI(
				pinModelDIR, childControllerId);
		}
		if (espinner_label == STEPPER_STEP_SELECTOR_LABEL) {

			espinnerStepper->setSTEP(espinner_value.toInt());
			ESP_PinMode pinModelSTEP =
				espinnerStepper->getPinModeConf(StepperPin::STEP);
			ESPAllOnPinManager::getInstance().updateGPIOFromESPUI(
				pinModelSTEP, childControllerId);
		}

		if (espinner_label == STEPPER_EN_SELECTOR_LABEL) {

			espinnerStepper->setEN(espinner_value.toInt());
			ESP_PinMode pinModelEN =
				espinnerStepper->getPinModeConf(StepperPin::EN);
			ESPAllOnPinManager::getInstance().updateGPIOFromESPUI(
				pinModelEN, childControllerId);
		}

		if (espinner_label == STEPPER_MODESELECTOR_LABEL) {
			espinnerStepper->setDriver(espinner_value);
		}

		if (espinner_label == STEPPER_CS_SELECTOR_LABEL) {

			espinnerStepper->setCS(espinner_value.toInt());
			espinnerStepper->setSPI(true);
			ESP_PinMode pinModelCS =
				espinnerStepper->getPinModeConf(StepperPin::CS);
			ESPAllOnPinManager::getInstance().updateGPIOFromESPUI(
				pinModelCS, childControllerId);
		}

		if (espinner_label == STEPPER_DIAG0_SELECTOR_LABEL) {

			espinnerStepper->setDIAG0(espinner_value.toInt());
			espinnerStepper->setISDIAG(true);
			ESP_PinMode pinModelDIAG0 =
				espinnerStepper->getPinModeConf(StepperPin::DIAG0);
			ESPAllOnPinManager::getInstance().updateGPIOFromESPUI(
				pinModelDIAG0, childControllerId);
		}

		if (espinner_label == STEPPER_DIAG1_SELECTOR_LABEL) {

			espinnerStepper->setDIAG1(espinner_value.toInt());
			espinnerStepper->setISDIAG(true);
			ESP_PinMode pinModelDIAG1 =
				espinnerStepper->getPinModeConf(StepperPin::DIAG1);
			ESPAllOnPinManager::getInstance().updateGPIOFromESPUI(
				pinModelDIAG1, childControllerId);
		}
	}
	// Create ESpinner with Configuration
	ESPinner_Manager::getInstance().push(std::move(espinnerStepper));
	ESPinner_Manager::getInstance().saveESPinnersInStorage();
}

uint16_t Stepper_driverSelector(uint16_t PIN_ptr) {
	String currentDriver = "UNKNOWN";
	uint16_t driverSelect = ESPUI.addControl(
		ControlType::Select, STEPPER_MODESELECTOR_LABEL, currentDriver,
		ControlColor::Wetasphalt, PIN_ptr, debugCallback);
	ESPUI.addControl(ControlType::Option, "AccelStepper",
					 getDriverName(Stepper_Driver::ACCELSTEPPER), None,
					 driverSelect);
	ESPUI.addControl(ControlType::Option, "A4988",
					 getDriverName(Stepper_Driver::A4988), None, driverSelect);
	ESPUI.addControl(ControlType::Option, "TMC2130",
					 getDriverName(Stepper_Driver::TMC2130), None,
					 driverSelect);
	ESPUI.addControl(ControlType::Option, "TMC2208",
					 getDriverName(Stepper_Driver::TMC2208), None,
					 driverSelect);
	ESPUI.addControl(ControlType::Option, "TMC2209",
					 getDriverName(Stepper_Driver::TMC2209), None,
					 driverSelect);
	ESPUI.addControl(ControlType::Option, "TMC2160",
					 getDriverName(Stepper_Driver::TMC2160), None,
					 driverSelect);
	ESPUI.addControl(ControlType::Option, "TMC2660",
					 getDriverName(Stepper_Driver::TMC2660), None,
					 driverSelect);
	ESPUI.addControl(ControlType::Option, "TMC5130",
					 getDriverName(Stepper_Driver::TMC5130), None,
					 driverSelect);
	ESPUI.addControl(ControlType::Option, "TMC5160",
					 getDriverName(Stepper_Driver::TMC5160), None,
					 driverSelect);

	ESPUI.addControl(ControlType::Option, "UNKNOWN",
					 getDriverName(Stepper_Driver::UNKNOWN), None,
					 driverSelect);
	addElementWithParent(elementToParentMap, driverSelect, PIN_ptr);
	return driverSelect;
}

void Stepper_Selector(uint16_t PIN_ptr) {

	Stepper_driverSelector(PIN_ptr);

	GUI_GPIOSetLabel(PIN_ptr, STEPPER_STEP_SELECT_LABEL,
					 STEPPER_STEP_SELECT_VALUE);
	GUI_GPIOSelector(PIN_ptr, STEPPER_STEP_SELECTOR_LABEL,
					 STEPPER_STEP_SELECTOR_VALUE, StepperSelector_callback);
	GUI_GPIOSetLabel(PIN_ptr, STEPPER_DIR_SELECT_LABEL,
					 STEPPER_DIR_SELECT_VALUE);
	GUI_GPIOSelector(PIN_ptr, STEPPER_DIR_SELECTOR_LABEL,
					 STEPPER_DIR_SELECTOR_VALUE, StepperSelector_callback);

	GUI_GPIOSetLabel(PIN_ptr, STEPPER_EN_SELECT_LABEL, STEPPER_EN_SELECT_VALUE);

	GUI_GPIOSelector(PIN_ptr, STEPPER_EN_SELECTOR_LABEL,
					 STEPPER_EN_SELECTOR_VALUE, StepperSelector_callback);

	if (false) {
		GUI_GPIOSetLabel(PIN_ptr, STEPPER_CS_SELECT_LABEL,
						 STEPPER_CS_SELECT_VALUE);
		GUI_GPIOSelector(PIN_ptr, STEPPER_CS_SELECTOR_LABEL,
						 STEPPER_CS_SELECTOR_VALUE, StepperSelector_callback);
	}
	if (false) {
		GUI_GPIOSetLabel(PIN_ptr, STEPPER_DIAG0_SELECT_LABEL,
						 STEPPER_DIAG0_SELECT_VALUE);
		GUI_GPIOSelector(PIN_ptr, STEPPER_DIAG0_SELECTOR_LABEL,
						 STEPPER_DIAG0_SELECTOR_VALUE,
						 StepperSelector_callback);
		GUI_GPIOSetLabel(PIN_ptr, STEPPER_DIAG1_SELECT_LABEL,
						 STEPPER_DIAG1_SELECT_VALUE);
		GUI_GPIOSelector(PIN_ptr, STEPPER_DIAG1_SELECTOR_LABEL,
						 STEPPER_DIAG1_SELECTOR_VALUE,
						 StepperSelector_callback);
	}
}

void removeStepper_callback(Control *sender, int type) {
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
			if (espinner_label == STEPPER_DIR_SELECTOR_LABEL) {
				detachRemovedPIN(STEPPER_DIR_SELECTOR_LABEL, espinner_label,
								 espinner_value);
			}
			if (espinner_label == STEPPER_STEP_SELECTOR_LABEL) {
				detachRemovedPIN(STEPPER_STEP_SELECTOR_LABEL, espinner_label,
								 espinner_value);
			}

			if (espinner_label == STEPPER_EN_SELECTOR_LABEL) {
				detachRemovedPIN(STEPPER_EN_SELECTOR_LABEL, espinner_label,
								 espinner_value);
			}

			if (espinner_label == STEPPER_CS_SELECTOR_LABEL) {
				detachRemovedPIN(STEPPER_CS_SELECTOR_LABEL, espinner_label,
								 espinner_value);
			}

			if (espinner_label == STEPPER_DIAG0_SELECTOR_LABEL) {
				detachRemovedPIN(STEPPER_DIAG0_SELECTOR_LABEL, espinner_label,
								 espinner_value);
			}
			if (espinner_label == STEPPER_DIAG1_SELECTOR_LABEL) {
				detachRemovedPIN(STEPPER_DIAG1_SELECTOR_LABEL, espinner_label,
								 espinner_value);
			}
			if (espinner_label == STEPPER_MODESELECTOR_LABEL) {
				// TODO: Review if this is the correct way to reset the stepper
				ESPinner *espinner =
					ESPinner_Manager::getInstance().findESPinnerById(
						espinner_label);
				if (espinner) {
					ESPinner_Stepper *espinnerStepper =
						static_cast<ESPinner_Stepper *>(espinner);
					espinnerStepper->stepper.reset();
				}
			}
		}
		removeElement_callback(sender, type);
	}
}

void updateStepperMotorState(uint16_t parentRef) {
	uint16_t PINSTEPSelectorRef =
		searchByLabel(parentRef, STEPPER_SWITCH_RUN_LABEL);
	uint16_t PINDIRSelectorRef =
		searchByLabel(parentRef, STEPPER_SWITCH_DIR_LABEL);

	uint8_t associatedPinSTEP =
		ESPAllOnPinManager::getInstance().getCurrentReference(
			PINSTEPSelectorRef);
	uint8_t associatedPinDIR =
		ESPAllOnPinManager::getInstance().getCurrentReference(
			PINDIRSelectorRef);

	DUMPSLN("Movement Begin: ");

	DUMPSLN("EndMovement: ");
}

void Stepper_UI(uint16_t Stepper_ptr) {
	Stepper_Selector(Stepper_ptr);
	GUIButtons_Elements(Stepper_ptr, STEPPER_SAVE_LABEL, STEPPER_SAVE_VALUE,
						STEPPER_REMOVE_LABEL, STEPPER_REMOVE_VALUE,
						saveStepper_callback, removeElement_callback);
}

void StepperSelector_callback(Control *sender, int type) {
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (isNumericAndInRange(sender->value, sender->id)) {

		bool isPinDIR = (String(sender->label) == STEPPER_DIR_SELECTOR_LABEL);
		bool isPinSTEP = (String(sender->label) == STEPPER_STEP_SELECTOR_LABEL);
		bool isPinEN = (String(sender->label) == STEPPER_EN_SELECTOR_LABEL);
		bool isPinCS = (String(sender->label) == STEPPER_CS_SELECTOR_LABEL);
		bool isPinDIAG0 =
			(String(sender->label) == STEPPER_DIAG0_SELECTOR_LABEL);
		bool isPinDIAG1 =
			(String(sender->label) == STEPPER_DIAG1_SELECTOR_LABEL);

		if (isPinDIR) {
			uint16_t selectLabelRefDIR =
				searchByLabel(parentRef, STEPPER_DIR_SELECT_LABEL);
			if (selectLabelRefDIR != 0) {
				ESPUI.removeControl(selectLabelRefDIR);
				removeValueFromMap(elementToParentMap, selectLabelRefDIR);
			}
		}

		if (isPinSTEP) {
			uint16_t selectLabelRefSTEP =
				searchByLabel(parentRef, STEPPER_STEP_SELECT_LABEL);
			if (selectLabelRefSTEP != 0) {
				ESPUI.removeControl(selectLabelRefSTEP);
				removeValueFromMap(elementToParentMap, selectLabelRefSTEP);
			}
		}

		if (isPinEN) {
			uint16_t selectLabelRefEN =
				searchByLabel(parentRef, STEPPER_EN_SELECT_LABEL);
			if (selectLabelRefEN != 0) {
				ESPUI.removeControl(selectLabelRefEN);
				removeValueFromMap(elementToParentMap, selectLabelRefEN);
			}
		}

		char *backgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(sender->id, backgroundStyle);

	} else {
		char *backgroundStyle = getBackground(DANGER_COLOR);
		ESPUI.setElementStyle(sender->id, backgroundStyle);
	}
}

void Stepper_VEL_Slider_callback(Control *sender, int type) {
	uint16_t parentRef =
		ESPinner_Manager::getInstance().findRefByControllerId(sender->id);
	// updateDCMotorState(parentRef);
}

void Stepper_DIR_Switcher_callback(Control *sender, int type) {
	uint16_t parentRef =
		ESPinner_Manager::getInstance().findRefByControllerId(sender->id);
	updateStepperMotorState(parentRef);
}

void Stepper_RUN_Switcher_callback(Control *sender, int type) {
	uint16_t parentRef =
		ESPinner_Manager::getInstance().findRefByControllerId(sender->id);
	updateStepperMotorState(parentRef);
}

void Stepper_Pad_callback(Control *sender, int type) {
	debugCallback(sender, type);

	uint16_t parentRef =
		ESPinner_Manager::getInstance().findRefByControllerId(sender->id);
	if (parentRef == 0)
		return;

	uint16_t runRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_SWITCH_RUN_LABEL);
	uint16_t dirRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_SWITCH_DIR_LABEL);
	uint16_t velRef =
		searchInMapByLabel(ESPinner_Manager::getInstance().getControllerMap(),
						   parentRef, STEPPER_SLIDER_VEL_LABEL);

	switch (type) {
	case P_CENTER_DOWN: // toggle RUN
		if (runRef) {
			String v = ESPUI.getControl(runRef)->value;
			ESPUI.updateSwitcher(runRef, v == "1" ? 0 : 1);
		}
		break;
	case P_LEFT_DOWN: // DIR = 0
		if (dirRef)
			ESPUI.updateSwitcher(dirRef, 0);
		break;
	case P_RIGHT_DOWN: // DIR = 1
		if (dirRef)
			ESPUI.updateSwitcher(dirRef, 1);
		break;
	case P_FOR_DOWN: // ++VEL
		if (velRef) {
			int val = ESPUI.getControl(velRef)->value.toInt();
			val = constrain(val + 5, 0, 100);
			ESPUI.updateSlider(velRef, val);
		}
		break;
	case P_BACK_DOWN: // --VEL
		if (velRef) {
			int val = ESPUI.getControl(velRef)->value.toInt();
			val = constrain(val - 5, 0, 100);
			ESPUI.updateSlider(velRef, val);
		}
		break;
	default:
		break;
	}
}

void Stepper_Controller(String ID_LABEL, uint16_t parentRef) {

	uint16_t controllerTabRef = getTab(TabType::ControllerTab);
	uint16_t STEPPER_PIN_ID = ESPUI.addControl(
		ControlType::Text, STEPPER_ID_LABEL, ID_LABEL.c_str(),
		ControlColor::Wetasphalt, controllerTabRef, debugCallback);
	ESPUI.getControl(STEPPER_PIN_ID)->enabled = false;

	uint16_t STEPPER_DIR_Switch =
		ESPUI.addControl(ControlType::Switcher, STEPPER_SWITCH_DIR_LABEL,
						 STEPPER_SWITCH_DIR_VALUE, ControlColor::Wetasphalt,
						 STEPPER_PIN_ID, Stepper_DIR_Switcher_callback);

	uint16_t STEPPER_RUN_Switch =
		ESPUI.addControl(ControlType::Switcher, STEPPER_SWITCH_RUN_LABEL,
						 STEPPER_SWITCH_RUN_VALUE, ControlColor::Wetasphalt,
						 STEPPER_PIN_ID, Stepper_RUN_Switcher_callback);

	uint16_t STEPPER_VEL_Slider = ESPUI.addControl(
		ControlType::Slider, STEPPER_SLIDER_VEL_LABEL, STEPPER_SLIDER_VEL_VALUE,
		ControlColor::Wetasphalt, STEPPER_PIN_ID, Stepper_VEL_Slider_callback);

	uint16_t STEPPER_PAD = ESPUI.addControl(
		ControlType::PadWithCenter, "Stepper Pad", "0",
		ControlColor::Wetasphalt, STEPPER_PIN_ID, Stepper_Pad_callback);

	ESPinner_Manager::getInstance().addControllerRelation(STEPPER_DIR_Switch,
														  parentRef);
	ESPinner_Manager::getInstance().addControllerRelation(STEPPER_RUN_Switch,
														  parentRef);
	ESPinner_Manager::getInstance().addControllerRelation(STEPPER_VEL_Slider,
														  parentRef);
	ESPinner_Manager::getInstance().addControllerRelation(STEPPER_PAD,
														  parentRef);

	ESPinner_Manager::getInstance().addUIRelation(parentRef, STEPPER_PIN_ID);
}

void saveStepper_callback(Control *sender, int type) {

	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	if (type == B_UP) {
		uint16_t StepperSelectorRefDIR =
			searchByLabel(parentRef, STEPPER_DIR_SELECTOR_LABEL);
		uint16_t StepperSelectorRefSTEP =
			searchByLabel(parentRef, STEPPER_STEP_SELECTOR_LABEL);
		uint16_t StepperSelectorRefEN =
			searchByLabel(parentRef, STEPPER_EN_SELECTOR_LABEL);

		uint16_t STEPPER_IDRef = searchByLabel(parentRef, ESPINNERID_LABEL);
		if (StepperSelectorRefDIR != 0 && StepperSelectorRefSTEP != 0 &&
			StepperSelectorRefEN != 0) {
			String Stepper_Selector_valueDIR =
				ESPUI.getControl(StepperSelectorRefDIR)->value;
			String Stepper_Selector_valueSTEP =
				ESPUI.getControl(StepperSelectorRefSTEP)->value;
			String Stepper_Selector_valueEN =
				ESPUI.getControl(StepperSelectorRefEN)->value;

			if (isNumericAndInRange(Stepper_Selector_valueDIR,
									StepperSelectorRefDIR) &&
				isNumericAndInRange(Stepper_Selector_valueSTEP,
									StepperSelectorRefSTEP) &&
				isNumericAndInRange(Stepper_Selector_valueEN,
									StepperSelectorRefEN)) {

				Stepper_action(parentRef);

				// ----- Create Controllers ------ //
				uint16_t controller = getParentId(
					ESPinner_Manager::getInstance().getUIRelationIDMap(),
					parentRef);
				if (controller == 0) {
					Stepper_Controller(ESPUI.getControl(STEPPER_IDRef)->value,
									   parentRef);
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

void ESPinner_Stepper::implement() {
	uint16_t parentRef = getTab(TabType::BasicTab);

	uint16_t Stepper_PIN_selector = ESPUI.addControl(
		ControlType::Text, ESPINNERID_LABEL, ESPinner_Stepper::getID(),
		ControlColor::Wetasphalt, parentRef, StepperSelector_callback);
	ESPUI.getControl(Stepper_PIN_selector)->enabled = false;
	addElementWithParent(elementToParentMap, Stepper_PIN_selector,
						 Stepper_PIN_selector);

	uint16_t driverSelector = Stepper_driverSelector(Stepper_PIN_selector);
	ESPUI.updateSelect(driverSelector, ESPinner_Stepper::get_driverName());

	String gpioDIR = String(ESPinner_Stepper::getDIR());
	uint16_t gpioDIR_ref =
		GUI_GPIOSelector(Stepper_PIN_selector, STEPPER_DIR_SELECTOR_LABEL,
						 gpioDIR.c_str(), StepperSelector_callback);

	String gpioSTEP = String(ESPinner_Stepper::getSTEP());
	uint16_t gpioSTEP_ref =
		GUI_GPIOSelector(Stepper_PIN_selector, STEPPER_STEP_SELECTOR_LABEL,
						 gpioSTEP.c_str(), StepperSelector_callback);

	String gpioEN = String(ESPinner_Stepper::getEN());
	uint16_t gpioEN_ref =
		GUI_GPIOSelector(Stepper_PIN_selector, STEPPER_EN_SELECTOR_LABEL,
						 gpioEN.c_str(), StepperSelector_callback);

	ESPAllOnPinManager::getInstance().setPinControlRelation(
		ESPinner_Stepper::getDIR(), gpioDIR_ref);
	ESPAllOnPinManager::getInstance().setPinControlRelation(
		ESPinner_Stepper::getSTEP(), gpioSTEP_ref);
	ESPAllOnPinManager::getInstance().setPinControlRelation(
		ESPinner_Stepper::getEN(), gpioEN_ref);

	ESP_PinMode pinModelDIR =
		ESP_PinMode(ESPinner_Stepper::getDIR(), OutputPin(), PinType::BusPWM);
	ESPAllOnPinManager::getInstance().attach(pinModelDIR);

	ESP_PinMode pinModelSTEP =
		ESP_PinMode(ESPinner_Stepper::getSTEP(), OutputPin(), PinType::BusPWM);
	ESPAllOnPinManager::getInstance().attach(pinModelSTEP);

	ESP_PinMode pinModelEN =
		ESP_PinMode(ESPinner_Stepper::getEN(), OutputPin(), PinType::BusPWM);
	ESPAllOnPinManager::getInstance().attach(pinModelEN);

	if (ESPinner_Stepper::getSPI()) {
		String gpioCS = String(ESPinner_Stepper::getCS());
		uint16_t gpioCS_ref =
			GUI_GPIOSelector(Stepper_PIN_selector, STEPPER_CS_SELECTOR_LABEL,
							 gpioCS.c_str(), StepperSelector_callback);

		ESPAllOnPinManager::getInstance().setPinControlRelation(
			ESPinner_Stepper::getCS(), gpioCS_ref);
		ESP_PinMode pinModelCS = ESP_PinMode(ESPinner_Stepper::getCS(),
											 OutputPin(), PinType::BusPWM);
		ESPAllOnPinManager::getInstance().attach(pinModelCS);
	}

	if (ESPinner_Stepper::getISDIAG()) {
		String gpioDIAG0 = String(ESPinner_Stepper::getDIAG0());
		uint16_t gpioDIAG0_ref =
			GUI_GPIOSelector(Stepper_PIN_selector, STEPPER_DIAG0_SELECTOR_LABEL,
							 gpioDIAG0.c_str(), StepperSelector_callback);
		String gpioDIAG1 = String(ESPinner_Stepper::getDIAG1());
		uint16_t gpioDIAG1_ref =
			GUI_GPIOSelector(Stepper_PIN_selector, STEPPER_DIAG1_SELECTOR_LABEL,
							 gpioDIAG1.c_str(), StepperSelector_callback);

		ESPAllOnPinManager::getInstance().setPinControlRelation(
			ESPinner_Stepper::getDIAG0(), gpioDIAG0_ref);
		ESP_PinMode pinModelDIAG0 = ESP_PinMode(ESPinner_Stepper::getDIAG0(),
												OutputPin(), PinType::BusPWM);
		ESPAllOnPinManager::getInstance().attach(pinModelDIAG0);

		ESPAllOnPinManager::getInstance().setPinControlRelation(
			ESPinner_Stepper::getDIAG1(), gpioDIAG1_ref);
		ESP_PinMode pinModelDIAG1 = ESP_PinMode(ESPinner_Stepper::getDIAG1(),
												OutputPin(), PinType::BusPWM);
		ESPAllOnPinManager::getInstance().attach(pinModelDIAG1);
	}

	GUIButtons_Elements(Stepper_PIN_selector, STEPPER_SAVE_LABEL,
						STEPPER_SAVE_VALUE, REMOVEESPINNER_LABEL,
						REMOVEESPINNER_VALUE, saveStepper_callback,
						removeStepper_callback);

	// ------- Create Controllers ----------- //
	Stepper_Controller(ESPinner_Stepper::getID(), Stepper_PIN_selector);
}

#endif