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

const ESPinner_Module *findModByName(const String &name) {
	for (const auto &module : mods) {
		if (module.name == name) {
			return &module; // Return pointer to mod
		}
	}
	return nullptr; // Return nullptr if not found
}

void createPINConfigCallback(Control *sender, int type) {
	debugCallback(sender, type);

	uint16_t parentRef = getParentId(elementToParentMap, sender->id);

	// Check if STANDARD SAVE Button exists on ESPinner selector
	uint16_t labelRef = searchByLabel(parentRef, "SelectorSave");
	DUMPLN("SAVE EXISTS?: ", labelRef);
	if (labelRef != 0) {
		ESPinnerSelector();
		uint16_t saveButton = searchByLabel(parentRef, "SelectorSave");
		if (saveButton != 0) {
			removeValueFromMap(elementToParentMap, saveButton);
			ESPUI.removeControl(saveButton);
		}
	}

	uint16_t ESPinnerTypeRef = searchByLabel(parentRef, "ESPinnerType");
	// If ESPinnerTypeRef is not VOID then remove content
	if (ESPinnerTypeRef != 0) {
		String ESPinnerType_value =
			String(ESPUI.getControl(ESPinnerTypeRef)->value);
		if (ESPinnerType_value != "VOID") {
		}
	}

	if (sender->value == "GPIO") {
#ifdef _ESPINNER_GPIO_H
		// If GPIO Model exists avoid duplicates
		uint16_t is_GPIO_Selector =
			searchByLabel(parentRef, "GPIO_ModeSelector");
		if (is_GPIO_Selector == 0) {
			// Remove previous Model in Espinner Selector

			GPIO_UI(parentRef);
		}
#endif
	}

	if (sender->value == "Stepper") {
#ifdef _ESPINNER_STEPPER_H
		uint16_t is_Stepper_Selector =
			searchByLabel(parentRef, "Stepper_Selector");
		if (is_Stepper_Selector == 0) {
			// Remove previous Model in Espinner Selector
			Stepper_UI(parentRef);
		}
#endif
	}

	if (sender->value == "DC") {
#ifdef _ESPINNER_DC_H
		// If DC Model exists avoid duplicates
		uint16_t is_DC_Selector = searchByLabel(parentRef, "DC_ModeSelector");
		if (is_DC_Selector == 0) {
			// Remove previous Model in Espinner Selector
			DC_UI(parentRef);
		}
#endif
	}

	if (sender->value == "MPU") {
#ifdef _ESPINNER_MPU_H
		// If MPU Model exists avoid duplicates
		uint16_t is_MPU_Selector = searchByLabel(parentRef, "MPU_ModeSelector");
		if (is_MPU_Selector == 0) {
			MPU_UI(parentRef);
		}
#endif
	}

	if (sender->value == "TFT") {
#ifdef _ESPINNER_TFT_H
		// If TFT Model exists avoid duplicates
		uint16_t is_TFT_Selector = searchByLabel(parentRef, "TFT_ModeSelector");
		if (is_TFT_Selector == 0) {
			TFT_UI(parentRef);
		}
#endif
	}

	if (sender->value == "Neopixel") {
#ifdef _ESPINNER_NEOPIXEL_H
		// If NEOPIXEL Model exists avoid duplicates
		uint16_t is_NEOPIXEL_Selector =
			searchByLabel(parentRef, "NP_ModeSelector");
		if (is_NEOPIXEL_Selector == 0) {
			NP_UI(parentRef);
		}
#endif
	}

	if (sender->value == "LCD") {
#ifdef _ESPINNER_LCD_H
		// If LCD Model exists avoid duplicates
		uint16_t is_LCD_Selector = searchByLabel(parentRef, "LCD_ModeSelector");
		if (is_LCD_Selector == 0) {
			LCD_UI(parentRef);
		}
#endif
	}

	if (sender->value == "RFID") {
#ifdef _ESPINNER_RFID_H
		// If RFID Model exists avoid duplicates
		uint16_t is_RFID_Selector =
			searchByLabel(parentRef, "RFID_ModeSelector");
		if (is_RFID_Selector == 0) {
			RFID_UI(parentRef);
		}
#endif
	}

	// Disable EspinnerType Selector and change Label on Parent
	uint16_t EspinnerTypeRef = searchByLabel(parentRef, "ESPinnerType");
	if (EspinnerTypeRef != 0) {
		String ESPinnerType_Label =
			String(ESPUI.getControl(EspinnerTypeRef)->value);
		const ESPinner_Module *ESPinnerMod = findModByName(ESPinnerType_Label);
		ESPUI.getControl(parentRef)->label = (ESPinnerMod->name).c_str();
		ESPUI.getControl(EspinnerTypeRef)->enabled = false;
	}
}

void setESPinner(uint16_t id) {
	uint16_t parentRef = getParentId(elementToParentMap, id);
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
			auto espinner_ptr = dynamic_cast<ESPinner_GPIO
		*>(gpio_espinner);

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
			// GPIO SELECTOR SHOULD UPDATE GPIO LIST
			removeGPIOLabel(espinner_value.toInt());
		}
	}
}

void saveElement_callback(Control *sender, int type) {
	DUMPLN("Label: ", sender->label);
	if (type == B_UP) {
		ESPinnerSelector();
		debugCallback(sender, type);
		// Review Parent in Selector and Review Spinner Model
		uint16_t parentRef = getParentId(elementToParentMap, sender->id);
		if (sender->value == "GPIO") {
#ifdef _ESPINNER_GPIO_H
			GPIO_UI(parentRef);
#endif
		}
		if (sender->value == "LCD") {
#ifdef _ESPINNER_LCD_H
			LCD_UI(parentRef);
#endif
		}
		if (sender->value == "MPU") {
#ifdef _ESPINNER_MPU_H
			MPU_UI(parentRef);
#endif
		}
		if (sender->value == "TFT") {
#ifdef _ESPINNER_TFT_H
			TFT_UI(parentRef);
#endif
		}
		if (sender->value == "Stepper") {
#ifdef _ESPINNER_STEPPER_H
			Stepper_UI(parentRef);
#endif
		}
		if (sender->value == "Encoder") {
#ifdef _ESPINNER_ENCODER_H
			Encoder_UI(parentRef);
#endif
		}
		if (sender->value == "RFID") {
#ifdef _ESPINNER_RFID_H
			RFID_UI(parentRef);
#endif
		}
	}
}

void removeElement_callback(Control *sender, int type) {
	if (type == B_UP) {
		debugCallback(sender, type);
		uint16_t parentRef = getParentId(elementToParentMap, sender->id);
		DUMPSLN("ELEMENTS SIZE ");

		std::vector<uint16_t> childrenIds =
			getChildrenIds(elementToParentMap, parentRef);
		for (uint16_t childControllerId : childrenIds) {
			String espinner_value =
				String(ESPUI.getControl(childControllerId)->value);
			String espinner_label =
				String(ESPUI.getControl(childControllerId)->label);

			if (espinner_label == "GPIO_PinSelector" ||
				espinner_label == "GPIO_PinInput") {
				DUMP("DETACH PIN ", espinner_value.toInt())
				ESPAllOnPinManager::getInstance().detach(
					espinner_value.toInt());
				// GPIO SELECTOR SHOULD UPDATE GPIO LIST
				std::string label = pinLabels[espinner_value.toInt() - 1];
				char *cstr = new char[label.length() + 1];
				strcpy(cstr, label.c_str());
				std::pair<uint8_t, const char *> newGPIO = {
					espinner_value.toInt(), cstr};
				addGPIOLabel(newGPIO);
			}
		}

		DUMPLN("ELEMENTS SIZE ", childrenIds.size());
		if (childrenIds.size() > 1) {

			ESPUI.removeControl(parentRef);
			removeControlId(controlReferences, parentRef);
			removeChildrenFromMap(elementToParentMap, parentRef);
		}
	}
}

/*----------------------------------------------------*/
/*----------------------------------------------------*/
/*----------------------------------------------------*/

// Create a selector Espinner Type and it executes callback to introduce a new
// Espinner Object.
void ESPinnerSelector() {
	uint16_t basicTabRef = getTab(TabType::BasicTab);
	int numElements = sizeof(mods) / sizeof(mods[0]);

	// Selector for ESPinner Type
	auto mainselector = ESPUI.addControl(ControlType::Select, "ESPinnerType",
										 "VOID", ControlColor::Wetasphalt,
										 basicTabRef, createPINConfigCallback);
	for (int i = 0; i < numElements; i++) {
		ESPUI.addControl(ControlType::Option, mods[i].name.c_str(),
						 mods[i].name, None, mainselector);
	}

	// ID Definition linked to ESPinner
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

	// Save Callback
	uint16_t GPIOAdd_selector = ESPUI.addControl(
		ControlType::Button, "SelectorSave", "Save", ControlColor::Alizarin,
		grandParentControl, saveElement_callback);

	addElementWithParent(elementToParentMap, GPIOAdd_selector,
						 grandParentControl); // Add Save Button to parent

	debugMap(elementToParentMap);
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

	// Link Actions Tab Where Actions are linked with configured device
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