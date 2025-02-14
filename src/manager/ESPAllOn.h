#ifndef _ESPALLON_H
#define _ESPALLON_H

#include "../controllers/ESPAllOn_Wifi.h"

#include "../controllers/ESPinner.h"
#include "../controllers/UI/TabController.h"
#include "../controllers/Wifi_Controller.h"
#include "../utils.h"

#include "../controllers/ESPAction.h"

#include <TickerFree.h>

/*----------------------------------------------------*/
/*----------------------------------------------------*/
/*----------------------------------------------------*/

void textCallback(Control *sender, int type) {
	// This callback is needed to handle the changed values, even though it
	// doesn't do anything itself.
}

void ESPinnerSelector();

void uiUpdate() {
	static uint16_t sliderVal = 10;

	// ESPUI.updateLabel(mainLabel, String(sliderVal));
}

void removeConfig(Control *sender, int type) {
	if (type == B_UP) {
		DUMPSLN("REMOVE PIN CONFIG");
		ESPinner_Manager::getInstance().clearPinConfigInStorage();
	}
}

/*----------------------------------------------------*/
/*----------------------------------------------------*/
/*----------------------------------------------------*/

TickerFree<> UI_UpdateTicker(uiUpdate, 500, 0, MILLIS);

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
		ESPALLON_Wifi::getInstance().wifi_ssid_text =
			ESPUI.addControl(ControlType::Text, SSID_LABEL, VOID_VALUE,
							 ControlColor::Alizarin, wifitab, textCallback);
		ESPUI.addControl(ControlType::Max, VOID_VALUE, "32", None,
						 ESPALLON_Wifi::getInstance().wifi_ssid_text);
		ESPALLON_Wifi::getInstance().wifi_pass_text =
			ESPUI.addControl(ControlType::Text, PASS_LABEL, VOID_VALUE,
							 ControlColor::Alizarin, wifitab, textCallback);
		ESPUI.addControl(ControlType::Max, VOID_VALUE, "64", None,
						 ESPALLON_Wifi::getInstance().wifi_pass_text);
		ESPUI.addControl(ControlType::Button, SAVE_LABEL, SAVE_VALUE,
						 ControlColor::Peterriver, wifitab,
						 enterWifiDetailsCallback);

		ESPUI.addControl(ControlType::Button, REMOVE_PINCONFIG_LABEL,
						 REMOVE_PINCONFIG_VALUE, ControlColor::Peterriver,
						 wifitab, removeConfig);
	}

	// Link Actions Tab Where Actions are linked with configured device
	void ESPActionSelector() {
		auto LinkActionTab = getTab(TabType::LinkedActions);

		uint16_t actionSelector = ESPUI.addControl(
			ControlType::Select, ACTION_LABEL, VOID_VALUE,
			ControlColor::Wetasphalt, LinkActionTab, generalCallback);

		for (const ESPAction &action : actions) {
			ESPUI.addControl(ControlType::Option, action.getName().c_str(),
							 action.getReference(), None, actionSelector);
		}
		for (const ESPAction &action : actions) {
			ESPUI.addControl(ControlType::Option, action.getName().c_str(),
							 action.getReference(), None, actionSelector);
		}

		uint16_t GPIO_Link_Action = ESPUI.addControl(
			ControlType::Button, LINKACTION_LABEL, LINKACTION_VALUE,
			ControlColor::Alizarin, actionSelector, voidCallback);
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