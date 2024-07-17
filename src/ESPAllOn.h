#ifndef _ESPALLON_H
#define _ESPALLON_H

#include "ESPinner.h"
#include <ESPUI.h>
#include <Ticker.h>

enum class ESPinner_Mod {
	VOID,
	GPIO,
	Stepper,
	RFID,
	NeoPixel,
	DC,
	Encoder,
	TFT,
	LCD
};

struct ESPinner_Module {
	ESPinner_Mod model;
	String name;
};

const ESPinner_Module mods[] = {{ESPinner_Mod::VOID, "VOID"},
								{ESPinner_Mod::GPIO, "GPIO"},
								{ESPinner_Mod::Stepper, "Stepper"},
								{ESPinner_Mod::RFID, "RFID"},
								{ESPinner_Mod::NeoPixel, "NeoPixel"},
								{ESPinner_Mod::DC, "DC"},
								{ESPinner_Mod::Encoder, "Encoder"},
								{ESPinner_Mod::TFT, "TFT"},
								{ESPinner_Mod::LCD, "LCD"}};

enum class TabType { BasicTab, AdvancedSettingsTab, NetworkTab };

struct TabController {
	TabType tab;
	char *description;
	uint16_t tabRef = 0;
};

TabController tabs[] = {{TabType::BasicTab, "Basic controls"},
						{TabType::AdvancedSettingsTab, "Advanced Settings"},
						{TabType::NetworkTab, "Wifi Credentials"}};

/*----------------------------------------------------*/
/*----------------   Vector List  --------------------*/
/*----------------------------------------------------*/

std::vector<uint16_t> controlReferences;

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

	Serial.println("Error: TabType not found!");
	return 0;
}
/*----------------------------------------------------*/
/*----------------------------------------------------*/
/*----------------------------------------------------*/

uint16_t mainLabel;

void uiUpdate() {
	static uint16_t sliderVal = 10;
	ESPUI.updateLabel(mainLabel, String(sliderVal));
}

Ticker UI_UpdateTicker(uiUpdate, 500, 0, MILLIS);
void ESPinnerSelector();

/*----------------------------------------------------*/
/*-------------------CallBacks------------------------*/
/*----------------------------------------------------*/
void voidCallback(Control *sender, int type) {}
void generalCallback(Control *sender, int type);
void extendedCallback(Control *sender, int type, void *param);
void generalCallback(Control *sender, int type) {
	Serial.print("CB: id(");
	Serial.print(sender->id);
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
}

void extendedCallback(Control *sender, int type, void *param) {
	Serial.print("CB: id(");
	Serial.print(sender->id);
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
	Serial.println(String("param = ") + String((int)param));
}

uint16_t getBeforeLastGPIOSelector() {
	if (controlReferences.size() >= 2) {
		return controlReferences[controlReferences.size() - 2];
	}
	Serial.println(
		"Error: Size of ControlReferences is less than two elements");
	return 0;
}

void createPINConfigCallback(Control *sender, int type) {
	Serial.print("CB: id(");
	Serial.print(sender->id);
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
	ESPinnerSelector();

	uint16_t previousRef = getBeforeLastGPIOSelector();

	// Removes callback from previous selector
	ESPUI.getControl(previousRef)->callback = voidCallback;
}

/*----------------------------------------------------*/
/*----------------------------------------------------*/
/*----------------------------------------------------*/

void ESPinnerSelector() {
	uint16_t basicTabRef = getTab(TabType::BasicTab);
	auto maintab = basicTabRef;

	int numElements = sizeof(mods) / sizeof(mods[0]);

	auto mainselector = ESPUI.addControl(ControlType::Select, "Selector",
										 "VOID", ControlColor::Wetasphalt,
										 maintab, createPINConfigCallback);
	for (int i = 0; i < numElements; i++) {
		ESPUI.addControl(ControlType::Option, mods[i].name.c_str(),
						 mods[i].name, None, mainselector);
	}

	controlReferences.push_back(mainselector);
}
/*----------------------------------------------------*/
/*----------------------------------------------------*/
/*----------------------------------------------------*/

class ESPAllOn {

  public:
	ESPAllOn()
		: capacity(MOD_CAPACITY), size(0),
		  modules(new ESPinner *[MOD_CAPACITY]) {}

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
	void mainTab() {}
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

	void begin() {
		wifiTab();
		ESPUI.begin(HOSTNAME);
	}

	void save() {

	};
	// Gestión de Debug
	void debug() {

	};
	void addDevice(ESPinner *device);

  private:
	PinManager<ESP_BoardConf, PinMode> *pinManager;
	int capacity;
	ESPinner **modules;
	int size;
};

extern ESPAllOn ESPALLON;
#endif