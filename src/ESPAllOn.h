#ifndef _ESPALLON_H
#define _ESPALLON_H

#include <ESPAllOnPinManager.h>

#include "ESPinner.h"
#include <ESPAction.h>
#include <ESPUI.h>
#include <Ticker.h>

enum class TabType { BasicTab, AdvancedSettingsTab, LinkedActions, NetworkTab };

struct TabController {
	TabType tab;
	const char *description;
	uint16_t tabRef = 0;
	TabController(TabType t, const char *desc, uint16_t ref = 0)
		: tab(t), description(desc), tabRef(ref) {}
};

TabController tabs[] = {{TabType::BasicTab, "Basic controls"},
						{TabType::LinkedActions, "Link Actions"},
						{TabType::AdvancedSettingsTab, "Advanced Settings"},
						{TabType::NetworkTab, "Wifi Credentials"}};

uint16_t getTab(const TabType &tabType) {

	for (auto &tabElement : tabs) {
		if (tabElement.tab == tabType) {
			if (tabElement.tabRef != 0) {
				return tabElement.tabRef;
			} else {
				tabElement.tabRef = ESPUI.addControl(ControlType::Tab, "",
													 tabElement.description);
			}
			return tabElement.tabRef;
		}
	}

	DUMPSLN("Error: TabType not found!");
	return 0;
}
/*----------------------------------------------------*/
/*----------------------------------------------------*/
/*----------------------------------------------------*/

void uiUpdate() {
	static uint16_t sliderVal = 10;

	// ESPUI.updateLabel(mainLabel, String(sliderVal));
}

Ticker UI_UpdateTicker(uiUpdate, 500, 0, MILLIS);
void ESPinnerSelector();

/*----------------------------------------------------*/
/*-------------------CallBacks------------------------*/
/*----------------------------------------------------*/

void debugCallback(Control *sender, int type) {
	DUMPPRINTLN();
	DUMP("CB: id(", sender->id);
	DUMP(") Type(", type);
	DUMP(") '", sender->label);
	DUMPLN("' = ", sender->value);
}

void voidCallback(Control *sender, int type) {}
void generalCallback(Control *sender, int type);
void extendedCallback(Control *sender, int type, void *param);
void generalCallback(Control *sender, int type) { debugCallback(sender, type); }

void extendedCallback(Control *sender, int type, void *param) {
	debugCallback(sender, type);
	DUMPLN("param = ", String((int)param));
}

uint16_t getBeforeLastGPIOSelector() {
	if (controlReferences.size() >= 2) {
		return controlReferences[controlReferences.size() - 2];
	}
	DUMPSLN("Error: Size of ControlReferences is less than two elements");
	return 0;
}

void PINConfigCallback(Control *sender, int type) {

	DUMPSLN("PIN CONFIG ");
	if (sender->value == "GPIO") {
#ifdef _ESPINNER_GPIO_H
		GPIO_UI(sender->id);
#endif
	}
}

void createPINConfigCallback(Control *sender, int type) {
	debugCallback(sender, type);
	ESPinnerSelector();

	uint16_t previousRef = getBeforeLastGPIOSelector();
	if (sender->value == "GPIO") {
#ifdef _ESPINNER_GPIO_H
		GPIO_UI(previousRef);
#endif
	}

	if (sender->value == "Stepper") {
#ifdef _ESPINNER_STEPPER_H
		// GPIO_UI(previousRef);
#endif
	}
	// Removes callback from previous selector
	ESPUI.getControl(previousRef)->callback = PINConfigCallback;
}

void saveElement_callback(Control *sender, int type) {
	debugCallback(sender, type);
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);
	std::vector<uint16_t> childrenIds =
		getChildrenIds(elementToParentMap, parentRef);

	ESPinner espinner;
	for (uint16_t childControllerId : childrenIds) {
		String espinner_value =
			String(ESPUI.getControl(childControllerId)->value);
		String espinner_label =
			String(ESPUI.getControl(childControllerId)->label);

		if (espinner_label == "ESPinnerType") {
			if (espinner_value == "GPIO") {
			}
			espinner.setType(ESPinner_Mod::GPIO);
		}

		if (espinner_label == "ESPinnerID") {
			espinner.setID(espinner_value);
		}

		// If ESPinner Model is in Mods , downcasting for details
		// implementation
		/* if (espinner.getType() == ESPinner_Mod::GPIO) {
			ESPinner *gpio_espinner = new ESPinner_GPIO();
			auto espinner_ptr = dynamic_cast<ESPinner_GPIO *>(gpio_espinner);

			if (espinner_label == "GPIO_ModeSelector") {
				if (espinner_label == "INPUT") {
					espinner_ptr->setGPIOMode(
						ESPinner_GPIOType::ESPINNER_INPUT);
				} else if (espinner_label == "GPIO_PinSelector") {
					espinner_ptr->setGPIOMode(
						ESPinner_GPIOType::ESPINNER_OUTPUT);
				}
			}
			if (espinner_label == "GPIO_PinSelector") {
				espinner_ptr->setGPIO(espinner_label.toInt());
			}
		} */
		if (espinner_label == "GPIO_PinSelector") {
			ESP_PinMode pinin = {espinner_value.toInt(),
								 InputPin(false, false),
								 PinType::BusDigital,
								 false,
								 true,
								 false};
			ESPAllOnPinManager::getInstance().attach(pinin);
		}
	}
}

void removeElement_callback(Control *sender, int type) {
	debugCallback(sender, type);
	uint16_t parentRef = getParentId(elementToParentMap, sender->id);

	std::vector<uint16_t> childrenIds =
		getChildrenIds(elementToParentMap, parentRef);
	for (uint16_t childControllerId : childrenIds) {
		String espinner_value =
			String(ESPUI.getControl(childControllerId)->value);
		String espinner_label =
			String(ESPUI.getControl(childControllerId)->label);

		if (espinner_label == "GPIO_PinSelector") {
			DUMP("DETACH PIN ", espinner_value.toInt())
			ESPAllOnPinManager::getInstance().detach(espinner_value.toInt());
		}
	}
	ESPUI.removeControl(parentRef);
	removeControlId(controlReferences, parentRef);
	removeValueFromMap(elementToParentMap, parentRef);
}

/*----------------------------------------------------*/
/*----------------------------------------------------*/
/*----------------------------------------------------*/

void ESPinnerSelector() {
	uint16_t basicTabRef = getTab(TabType::BasicTab);

	int numElements = sizeof(mods) / sizeof(mods[0]);

	auto mainselector = ESPUI.addControl(ControlType::Select, "ESPinnerType",
										 "VOID", ControlColor::Wetasphalt,
										 basicTabRef, createPINConfigCallback);
	for (int i = 0; i < numElements; i++) {
		ESPUI.addControl(ControlType::Option, mods[i].name.c_str(),
						 mods[i].name, None, mainselector);
	}
	auto mainText = ESPUI.addControl(Text, "ESPinnerID", "ID", Alizarin,
									 mainselector, generalCallback);

	uint16_t parentControl = ESPUI.getControl(mainText)->id;
	uint16_t grandParentControl =
		ESPUI.getControl(parentControl)->parentControl;

	controlReferences.push_back(mainselector);

	addElementWithParent(elementToParentMap, mainselector,
						 grandParentControl); // Add Selector to parent
	addElementWithParent(elementToParentMap, mainText,
						 grandParentControl); // Add ID to parent
}
/*----------------------------------------------------*/
/*----------------------------------------------------*/
/*----------------------------------------------------*/

class ESPAllOn {

  public:
	ESPAllOn()
		: capacity(MOD_CAPACITY), size(0),
		  modules(new ESPinner *[MOD_CAPACITY]) {}

	static ESPAllOn &getInstance() {
		static ESPAllOn instance;
		return instance;
	}
	void setup() {

#ifdef ESP8266
		{
			HeapSelectIram doAllocationsInIRAM;
#endif
			ESPinnerSelector();

#ifdef ESP8266
		} // HeapSelectIram
#endif
	}
	void wifiTab() {
		auto wifitab = getTab(TabType::NetworkTab);
		wifi_ssid_text =
			ESPUI.addControl(ControlType::Text, "SSID", "",
							 ControlColor::Alizarin, wifitab, textCallback);
		ESPUI.addControl(ControlType::Max, "", "32", None, wifi_ssid_text);
		wifi_pass_text =
			ESPUI.addControl(ControlType::Text, "Password", "",
							 ControlColor::Alizarin, wifitab, textCallback);
		ESPUI.addControl(ControlType::Max, "", "64", None, wifi_pass_text);
		ESPUI.addControl(ControlType::Button, "Save", "Save",
						 ControlColor::Peterriver, wifitab,
						 enterWifiDetailsCallback);
	}

	void ESPActionSelector() {
		auto LinkActionTab = getTab(TabType::LinkedActions);

		uint16_t actionSelector = ESPUI.addControl(
			ControlType::Select, "Action", "", ControlColor::Wetasphalt,
			LinkActionTab, generalCallback);

		for (const ESPAction &action : actions) {
			ESPUI.addControl(ControlType::Option, action.getName().c_str(),
							 action.getReference(), None, actionSelector);
		}
		for (const ESPAction &action : actions) {
			ESPUI.addControl(ControlType::Option, action.getName().c_str(),
							 action.getReference(), None, actionSelector);
		}

		uint16_t GPIO_Link_Action = ESPUI.addControl(
			ControlType::Button, "Link", "Link", ControlColor::Alizarin,
			actionSelector, voidCallback);
	}

	void begin() {
		linkItemsTab();

		wifiTab();
		ESPUI.begin(HOSTNAME);
	}

	void save() {

	};

	void linkItemsTab() { ESPActionSelector(); }
	// Gestión de Debug
	void debug() {

	};
	void addDevice(ESPinner *device);
	void addAction(const ESPAction &action) { actions.push_back(action); }
	void assignActionToPin(uint16_t pin, const String &actionName) {
		for (const ESPAction &action : actions) {
			// if (action.getName() == actionName) {
			// pinActions[pin] = action;
			// break;
			// }
		}
	}

	void triggerPin(uint16_t pin) {
		auto it = pinActions.find(pin);
		if (it != pinActions.end()) {
			it->second.execute(12);
		}
	}

  private:
	ESPAllOnPinManager *pinManager;
	int capacity;
	ESPinner **modules;
	int size;
	std::vector<ESPAction> actions;
	std::map<uint16_t, ESPAction> pinActions;
};

#endif