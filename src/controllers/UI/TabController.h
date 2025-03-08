#ifndef _ESPALLON_TABCONTROLLER_H
#define _ESPALLON_TABCONTROLLER_H
#include <ESPUI.h>
#include <TickerFree.h>

#include "../../utils.h"

void ESPinnerSelector();

enum class TabType {
	BasicTab,
	ControllerTab,
	AdvancedSettingsTab,
	LinkedActions,
	NetworkTab
};

struct TabController {
	TabType tab;
	const char *description;
	uint16_t tabRef = 0;
	TabController(TabType t, const char *desc, uint16_t ref = 0)
		: tab(t), description(desc), tabRef(ref) {}
};

TabController tabs[] = {{TabType::BasicTab, BASICTAB_LABEL},
						{TabType::ControllerTab, CONTROLLERTAB_LABEL},
						{TabType::LinkedActions, LINKACTIONSTAB_LABEL},
						{TabType::AdvancedSettingsTab, ADVCONFIGTAB_LABEL},
						{TabType::NetworkTab, WIFITAB_LABEL}};

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
void ESPINNER_ID_Callback(Control *sender, int type);
void generalCallback(Control *sender, int type) { debugCallback(sender, type); }

void extendedCallback(Control *sender, int type, void *param) {
	debugCallback(sender, type);
	DUMPLN("param = ", String((int)param));
}

const ESPinner_Module *findModByName(const String &name) {
	for (const auto &module : mods) {
		if (module.name == name) {
			return &module; // Return pointer to mod
		}
	}
	return nullptr; // Return nullptr if not found
}

#include "../../manager/ESPinner_Manager.h"

void createPINConfigCallback(Control *sender, int type) {
	debugCallback(sender, type);

	uint16_t parentRef = getParentId(elementToParentMap, sender->id);

	// Check if STANDARD SAVE Button exists on ESPinner selector
	uint16_t labelRef = searchByLabel(parentRef, SAVEESPINNER_LABEL);
	if (labelRef != 0) {
		ESPinnerSelector();
		uint16_t saveButton = searchByLabel(parentRef, SAVEESPINNER_LABEL);
		if (saveButton != 0) {
			removeValueFromMap(elementToParentMap, saveButton);
			ESPUI.removeControl(saveButton);
		}
	}

	uint16_t ESPinnerTypeRef = searchByLabel(parentRef, ESPINNERTYPE_LABEL);
	// If ESPinnerTypeRef is not VOID then remove content
	if (ESPinnerTypeRef != 0) {
		String ESPinnerType_value =
			String(ESPUI.getControl(ESPinnerTypeRef)->value);
		if (ESPinnerType_value != VOID_LABEL) {
		}
	}

	if (sender->value == GPIO_LABEL) {
#ifdef _ESPINNER_GPIO_H
		// If GPIO Model exists avoid duplicates
		uint16_t is_GPIO_Selector =
			searchByLabel(parentRef, GPIO_MODESELECTOR_LABEL);
		if (is_GPIO_Selector == 0) {
			// Remove previous Model in Espinner Selector

			GPIO_UI(parentRef);
		}
#endif
	}

	if (sender->value == STEPPER_LABEL) {
#ifdef _ESPINNER_STEPPER_H
		uint16_t is_Stepper_Selector =
			searchByLabel(parentRef, STEPPER_MODESELECTOR_LABEL);
		if (is_Stepper_Selector == 0) {
			// Remove previous Model in Espinner Selector
			Stepper_UI(parentRef);
		}
#endif
	}

	if (sender->value == DC_LABEL) {
#ifdef _ESPINNER_DC_H
		// If DC Model exists avoid duplicates
		uint16_t is_DC_Selector =
			searchByLabel(parentRef, DC_MODESELECTOR_LABEL);
		if (is_DC_Selector == 0) {
			// Remove previous Model in Espinner Selector
			DC_UI(parentRef);
		}
#endif
	}

	if (sender->value == MPU_LABEL) {
#ifdef _ESPINNER_MPU_H
		// If MPU Model exists avoid duplicates
		uint16_t is_MPU_Selector =
			searchByLabel(parentRef, MPU_MODESELECTOR_LABEL);
		if (is_MPU_Selector == 0) {
			MPU_UI(parentRef);
		}
#endif
	}

	if (sender->value == TFT_LABEL) {
#ifdef _ESPINNER_TFT_H
		// If TFT Model exists avoid duplicates
		uint16_t is_TFT_Selector =
			searchByLabel(parentRef, TFT_MODESELECTOR_LABEL);
		if (is_TFT_Selector == 0) {
			TFT_UI(parentRef);
		}
#endif
	}

	if (sender->value == NEOPIXEL_LABEL) {
#ifdef _ESPINNER_NEOPIXEL_H
		// If NEOPIXEL Model exists avoid duplicates
		uint16_t is_NEOPIXEL_Selector =
			searchByLabel(parentRef, NEOPIXEL_MODESELECTOR_LABEL);
		if (is_NEOPIXEL_Selector == 0) {
			NP_UI(parentRef);
		}
#endif
	}

	if (sender->value == LCD_LABEL) {
#ifdef _ESPINNER_LCD_H
		// If LCD Model exists avoid duplicates
		uint16_t is_LCD_Selector =
			searchByLabel(parentRef, LCD_MODESELECTOR_LABEL);
		if (is_LCD_Selector == 0) {
			LCD_UI(parentRef);
		}
#endif
	}

	if (sender->value == ENCODER_LABEL) {
#ifdef _ESPINNER_ENCODER_H
		// If RFID Model exists avoid duplicates
		uint16_t is_ENCODER_Selector =
			searchByLabel(parentRef, ENCODER_MODESELECTOR_LABEL);
		if (is_ENCODER_Selector == 0) {
			Encoder_UI(parentRef);
		}
#endif
	}

	if (sender->value == RFID_LABEL) {
#ifdef _ESPINNER_RFID_H
		// If RFID Model exists avoid duplicates
		uint16_t is_RFID_Selector =
			searchByLabel(parentRef, RFID_MODESELECTOR_LABEL);
		if (is_RFID_Selector == 0) {
			RFID_UI(parentRef);
		}
#endif
	}

	// Disable EspinnerType Selector and change Label on Parent
	uint16_t EspinnerTypeRef = searchByLabel(parentRef, ESPINNERTYPE_LABEL);
	if (EspinnerTypeRef != 0) {
		String ESPinnerType_Label =
			String(ESPUI.getControl(EspinnerTypeRef)->value);
		const ESPinner_Module *ESPinnerMod = findModByName(ESPinnerType_Label);
		ESPUI.getControl(parentRef)->label = (ESPinnerMod->name).c_str();
		ESPUI.getControl(EspinnerTypeRef)->enabled = false;
	}
}

// Create a selector Espinner Type and it executes callback to introduce a new
// Espinner Object.
void ESPinnerSelector() {
	uint16_t basicTabRef = getTab(TabType::BasicTab);
	int numMods = sizeof(mods) / sizeof(mods[0]);

	// Selector for ESPinner Type
	auto mainselector = ESPUI.addControl(
		ControlType::Select, ESPINNERTYPE_LABEL, ESPINNERTYPE_VALUE,
		ControlColor::Turquoise, basicTabRef, createPINConfigCallback);
	for (int i = 0; i < numMods; i++) {
		ESPUI.addControl(ControlType::Option, mods[i].name.c_str(),
						 mods[i].name, None, mainselector);
	}

	// ID Definition linked to ESPinner
	std::string uniqueID =
		std::string(ESPINNERID_VALUE) + std::to_string(mainselector);
	auto mainText = ESPUI.addControl(ControlType::Text, ESPINNERID_LABEL,
									 uniqueID.c_str(), ControlColor::Alizarin,
									 mainselector, ESPINNER_ID_Callback);

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
		ControlType::Button, SAVEESPINNER_LABEL, SAVEESPINNER_VALUE,
		ControlColor::Alizarin, grandParentControl, saveElement_callback);

	addElementWithParent(elementToParentMap, GPIOAdd_selector,
						 grandParentControl); // Add Save Button to parent
}

void saveElement_callback(Control *sender, int type) {
	if (type == B_UP) {
		ESPinnerSelector();
		debugCallback(sender, type);
		// Review Parent in Selector and Review ESPinner Model
		uint16_t parentRef = getParentId(elementToParentMap, sender->id);
		if (sender->value == GPIO_LABEL) {
#ifdef _ESPINNER_GPIO_H
			GPIO_UI(parentRef);
			char *backgroundStyle = getBackground(PENDING_COLOR);
			ESPUI.setPanelStyle(parentRef, backgroundStyle);
#endif
		}

		if (sender->value == DC_LABEL) {
#ifdef _ESPINNER_DC_H
			DC_UI(parentRef);
			DUMPSLN("DC MOTOR");
			char *backgroundStyle = getBackground(PENDING_COLOR);
			ESPUI.setPanelStyle(parentRef, backgroundStyle);
#endif
		}

		if (sender->value == NEOPIXEL_LABEL) {
#ifdef _ESPINNER_NEOPIXEL_H
			NP_UI(parentRef);
#endif
		}
		if (sender->value == LCD_LABEL) {
#ifdef _ESPINNER_LCD_H
			LCD_UI(parentRef);
#endif
		}
		if (sender->value == MPU_LABEL) {
#ifdef _ESPINNER_MPU_H
			MPU_UI(parentRef);
#endif
		}
		if (sender->value == TFT_LABEL) {
#ifdef _ESPINNER_TFT_H
			TFT_UI(parentRef);
#endif
		}
		if (sender->value == STEPPER_LABEL) {
#ifdef _ESPINNER_STEPPER_H
			Stepper_UI(parentRef);
#endif
		}
		if (sender->value == ENCODER_LABEL) {
#ifdef _ESPINNER_ENCODER_H
			Encoder_UI(parentRef);
#endif
		}
		if (sender->value == RFID_LABEL) {
#ifdef _ESPINNER_RFID_H
			RFID_UI(parentRef);
#endif
		}
	}
}

void removeElement_callback(Control *sender, int type) {
	if (type == B_UP) {
		uint16_t parentRef = getParentId(elementToParentMap, sender->id);
		std::vector<uint16_t> childrenIds =
			getChildrenIds(elementToParentMap, parentRef);

		if (childrenIds.size() > 1) {
			uint16_t espinnerID_ref =
				searchByLabel(parentRef, ESPINNERID_LABEL);
			Control *IDController = ESPUI.getControl(espinnerID_ref);
			ESPinner_Manager::getInstance().debug();
			if (espinnerID_ref != 0) {
				ESPinner_Manager::getInstance().detach(IDController->value);
			} else {
				DUMPLN("NOT FOUND TO ERASE: ", espinnerID_ref);
			}

			ESPUI.removeControl(parentRef);
			removeControlId(controlReferences, parentRef);
			removeChildrenFromMap(elementToParentMap, parentRef);
		}
		ESPinner_Manager::getInstance().saveESPinnersInStorage();
	}
}

void detachRemovedPIN(String expected_label, String espinner_label,
					  String espinner_value) {
	if (espinner_label == expected_label) {

		DUMP("DETACH PIN ", espinner_value.toInt())
		ESPAllOnPinManager::getInstance().detach(espinner_value.toInt());
	}
}
/*----------------------------------------------------*/
/*----------------------------------------------------*/
/*----------------------------------------------------*/

#endif