#ifndef _ESPINNER_MANAGER_H
#define _ESPINNER_MANAGER_H

#include "../config.h"
#include "Storage_Manager.h"
#include <Persistance.h>
#include <list>

#include "../controllers/ESPinner.h"

#include "../controllers/UI/TabController.h"
#include "./upcast/upcast_utils.h"
class ESPinner_Manager {
  private:
	std::list<std::unique_ptr<ESPinner>> ESPinners;
	Persistance ESPinnerManager;
	ESPAllOnPinManager *pinManager;

	// Relation between controller in ESPinner
	// Controller Selector - ESPinner Selector
	// ( Espinner could have several controllers)
	std::map<uint16_t, uint16_t> ESPinnerPINController;

	// External Ref - Parent Ref
	std::map<uint16_t, uint16_t> UI_relationID;

	// Direct mapping: ESPinner ID String -> Controller UI Reference
	// This provides O(1) lookup for controller access
	std::map<String, uint16_t> ESPinnerID_to_ControllerRef;

  public:
	ESPinner_Manager() : ESPinnerManager(nullptr, &storage) {
		storage.setRoot(ESPinner_File);
		pinManager = &ESPAllOnPinManager::getInstance();
	}
	static ESPinner_Manager &getInstance() {
		static ESPinner_Manager instance;
		return instance;
	}
	size_t espinnerSize() const { return ESPinners.size(); }
	void clearESPinners() {
		ESPinners.clear();
		clearESPinnerControllerMappings();
	}

	std::map<uint16_t, uint16_t> &getControllerMap() {
		return ESPinnerPINController;
	}
	std::map<uint16_t, uint16_t> &getUIRelationIDMap() { return UI_relationID; }

	void loadFromStorage() {
		DynamicJsonDocument doc(256);
		String serialized = ESPinnerManager.loadData(ESPinner_Path);
		DUMP("Dataloaded: ", serialized);
		DeserializationError error = deserializeJson(doc, serialized);
		if (!error) {
			if (!doc.is<JsonArray>()) {
				DUMPSLN("ERROR: NO JSON ARRAY FORMAT.");
				return;
			}
			clearESPinners();
			JsonArray array = doc.as<JsonArray>();
			for (JsonDocument obj : array) {
				String mod = obj[ESPINNER_MODEL_JSONCONFIG].as<const char *>();

				DUMPLN("MOD: ", mod);
				auto espinner = ESPinner::create(mod);
				if (espinner) {
					String output;
					DUMPLN("ESPinner loaded: ", mod);
					serializeJson(obj, output);
					espinner->deserializeJSON(output);

					uint16_t parentRef = getTab(TabType::BasicTab);

					// Implement Includes GUI and ESPAllOn_PinManager
					// Configuration
					espinner->implement();

					// Create ESPinner Model in List
					ESPinners.push_back(std::move(espinner));
					// ESPinner_Manager::getInstance().debug();
				}
			}
		}
	}

	/**
	 * Load ESPinners from JSON string
	 * @param jsonString JSON string containing ESPinner configuration array
	 * @return true if loaded successfully, false otherwise
	 */
	bool loadFromJSON(const String &jsonString) {
		DynamicJsonDocument doc(2048);
		DeserializationError error = deserializeJson(doc, jsonString);
		if (error) {
			DUMPSLN("JSON parsing error in loadFromJSON");
			return false;
		}

		if (!doc.is<JsonArray>()) {
			DUMPSLN("ERROR: JSON is not an array format in loadFromJSON.");
			return false;
		}

		clearESPinners();
		JsonArray array = doc.as<JsonArray>();
		uint16_t idx = 0;
		for (JsonDocument obj : array) {
			String mod = obj[ESPINNER_MODEL_JSONCONFIG].as<const char *>();

			char where[32];
			snprintf(where, sizeof(where), "loop %u pre", idx);
			dumpHeap(where);
			DUMPLN("Loading ESPinner module from JSON: ", mod);

			auto espinner = ESPinner::create(mod);
			if (espinner) {
				String output;
				serializeJson(obj, output);
				espinner->deserializeJSON(output);
				uint16_t parentRef = getTab(TabType::BasicTab);

				// Implement Includes GUI and ESPAllOn_PinManager Configuration
				espinner->implement();
				dumpHeap("after implement");
				// Create ESPinner Model in List
				ESPinners.push_back(std::move(espinner));
				dumpHeap("after push_back");
			} else {
				DUMPLN("Failed to create ESPinner for module: ", mod);
			}
			idx++;
		}

		// Save the new configuration to storage
		saveESPinnersInStorage();

		return true;
	}

	void saveESPinnersInStorage() {
		DynamicJsonDocument doc(1024);
		JsonArray JSONESPinner_array = doc.to<JsonArray>();
		for (const auto &espinner : ESPinners) {
			JsonDocument JSONEspinner = espinner->serializeJSON();
			JSONESPinner_array.add(JSONEspinner);
		}

		String data;
		serializeJson(doc, data);
		ESPinnerManager.getStorageModel()->save(data, ESPinner_Path);
	}

	void clearPinConfigInStorage() {
		ESPinnerManager.getStorageModel()->remove(ESPinner_Path);
	}

	// ------------------------------------- //
	// -------- CONTROLLER REF MAP --------- //
	// ------------------------------------- //

	uint16_t findRefByControllerId(uint16_t controllerId) {
		return findByKey(ESPinnerPINController, controllerId,
						 static_cast<uint16_t>(0));
	}

	void addControllerRelation(uint16_t espinnerId, uint16_t controllerId) {
		addRelation(ESPinnerPINController, espinnerId, controllerId);
	}

	void removeController(uint16_t controller) {
		removeByKey(ESPinnerPINController, controller);
	}

	void removeAllControllersBySelector(uint16_t selector) {
		removeByKey(ESPinnerPINController, selector);
	}

	void removeAllControllersByParent(uint16_t parentRef) {
		removeByValue(ESPinnerPINController, parentRef);
	}

	void debugController() {
		debugGenericMap(ESPinnerPINController, "ESPinnerPINController");
	}

	// ------------------------------------- //
	// ---------- UI RELATION MAP ---------- //
	// ------------------------------------- //

	uint16_t findUIRelationRefByID(uint16_t controllerId) {
		return findByKey(UI_relationID, controllerId, static_cast<uint16_t>(0));
	}

	void addUIRelation(uint16_t parentRefID, uint16_t UI_ID) {
		addRelation(UI_relationID, parentRefID, UI_ID);
	}

	void removeUIRelation(uint16_t UI_ID) { removeByKey(UI_relationID, UI_ID); }

	void removeAllUIRelationBySelector(uint16_t selector) {
		removeByKey(UI_relationID, selector);
	}

	void debugUIRelation() { debugGenericMap(UI_relationID, "UI_relationID"); }

	// ------------------------------------- //
	// ------- ESPinner ID TO CONTROLLER --- //
	// ------------------------------------- //

	/**
	 * Add direct mapping from ESPinner ID to its controller reference
	 * This enables O(1) lookup for controller access
	 * @param espinnerID String ID of the ESPinner
	 * @param controllerRef UI reference of the controller
	 */
	void addESPinnerControllerMapping(const String &espinnerID,
									  uint16_t controllerRef) {
		addRelation(ESPinnerID_to_ControllerRef, espinnerID, controllerRef);
	}

	/**
	 * Get controller reference by ESPinner ID
	 * @param espinnerID String ID of the ESPinner
	 * @return Controller reference or 0 if not found
	 */
	uint16_t getControllerRefByESPinnerID(const String &espinnerID) {
		return findByKey(ESPinnerID_to_ControllerRef, espinnerID,
						 static_cast<uint16_t>(0));
	}

	/**
	 * Remove ESPinner controller mapping
	 * @param espinnerID String ID of the ESPinner to remove
	 */
	void removeESPinnerControllerMapping(const String &espinnerID) {
		removeByKey(ESPinnerID_to_ControllerRef, espinnerID);
	}

	/**
	 * Clear all ESPinner controller mappings
	 */
	void clearESPinnerControllerMappings() {
		ESPinnerID_to_ControllerRef.clear();
	}

	void debugESPinnerControllerMap() {
		debugGenericMap(ESPinnerID_to_ControllerRef,
						"ESPinnerID_to_ControllerRef");
	}

	// ------------------------------------- //
	// ---------- ESPINNER METHODS --------- //
	// ------------------------------------- //

	ESPinner *findESPinnerById(const String &id) {
		for (auto &espinner : ESPinners) {
			if (espinner->ID == id) {
				return espinner.get();
			}
		}
		return nullptr;
	}

	void push(std::unique_ptr<ESPinner> GUIESPinner) {
		auto it = std::find_if(
			ESPinners.begin(), ESPinners.end(),
			[&GUIESPinner](const std::unique_ptr<ESPinner> &espinner) {
				return espinner->getID() == GUIESPinner->getID();
			});

		if (it != ESPinners.end()) {
			*it = std::move(GUIESPinner);
		} else {
			ESPinners.push_back(std::move(GUIESPinner));
		}
		saveESPinnersInStorage();
	}

	void debug() {
		for (auto &espinner : ESPinners) {
			DUMPLN("ID FROM ESPINNER: ", espinner->getID());
		}
	}

	void detach(const String &id) {
		auto it =
			std::find_if(ESPinners.begin(), ESPinners.end(),
						 [&id](const std::unique_ptr<ESPinner> &espinner) {
							 return espinner->getID() == id;
						 });
		if (it != ESPinners.end()) {
			ESPinners.erase(it);
		} else {
			DUMPSLN("ESPINNER NOT ERASED IN DETACH");
		}
	}
};

void ESPINNER_ID_Callback(Control *sender, int type) {
	// Search if ID is already assigned
	debugCallback(sender, type);

	ESPinner *espinnerInList =
		ESPinner_Manager::getInstance().findESPinnerById(sender->value);
	if (espinnerInList == nullptr) {
		char *backgroundStyle = getBackground(SUCCESS_COLOR);
		// ESPUI.setElementStyle(sender->id, backgroundStyle);
	} else {
		char *backgroundStyle = getBackground(DANGER_COLOR);
		ESPUI.setElementStyle(sender->id, backgroundStyle);
	}
}

AccelStepperAdapter *findStepperAdapterById(const String &id) {
	ESPinner *espinner = ESPinner_Manager::getInstance().findESPinnerById(id);
	if (espinner && espinner->getType() == ESPinner_Mod::Stepper) {

		ESPinner_Stepper *stepperESPinner =
			static_cast<ESPinner_Stepper *>(espinner);
		return static_cast<AccelStepperAdapter *>(
			stepperESPinner->stepper.get());
	}
	return nullptr;
}

#include "../mods/ESPinner_DC/ESPinner_DC_Controls.h"
#include "../mods/ESPinner_GPIO/ESPinner_GPIO_Controls.h"
#include "../mods/ESPinner_Stepper/ESPinner_Stepper_Controls.h"

#include "../mods/ESPinner_Encoder/ESPinner_Encoder_Controls.h"
#include "../mods/ESPinner_LCD/ESPinner_LCD_Controls.h"
#include "../mods/ESPinner_MPU/ESPinner_MPU_Controls.h"
#include "../mods/ESPinner_NeoPixel/ESPinner_NeoPixel_Controls.h"
#include "../mods/ESPinner_RFID/ESPinner_RFID_Controls.h"
#include "../mods/ESPinner_TFT/ESPinner_TFT_Controls.h"

#endif