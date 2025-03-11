#ifndef _ESPINNER_MANAGER_H
#define _ESPINNER_MANAGER_H

#include "../config.h"
#include "Storage_Manager.h"
#include <Persistance.h>

#include "../controllers/ESPinner.h"

#include "../controllers/UI/TabController.h"
#include "./upcast/upcast_utils.h"
class ESPinner_Manager {
  private:
	std::list<std::unique_ptr<ESPinner>> ESPinners;
	Persistance ESPinnerManager;
	ESPAllOnPinManager *pinManager;

	// Relation between controller in ESPinner
	// Controller Selector - ESPinner Selector  ( Espinner could have several
	// controllers)

  public:
	std::map<uint16_t, uint16_t> ESPinnerController;
	ESPinner_Manager() : ESPinnerManager(nullptr, &storage) {
		storage.setRoot(ESPinner_File);
		pinManager = &ESPAllOnPinManager::getInstance();
	}
	static ESPinner_Manager &getInstance() {
		static ESPinner_Manager instance;
		return instance;
	}
	size_t espinnerSize() const { return ESPinners.size(); }
	void clearESPinners() { ESPinners.clear(); }

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

	ESPinner *findESPinnerById(const String &id) {
		for (auto &espinner : ESPinners) {
			if (espinner->ID == id) {
				return espinner.get();
			}
		}
		return nullptr;
	}

	uint16_t findBySelectorId(uint16_t selectorId) {
		return ESPinnerController[selectorId];
	}

	void debugController() {
		for (const auto &pair : ESPinnerController) {
			DUMPLN("ID FROM ESPINNER: ", pair.first);
			DUMPLN("ID FROM CONTROLLER: ", pair.second);
		}
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

	void addControllerRelation(uint16_t espinnerId, uint16_t controllerId) {
		ESPinnerController[controllerId] = espinnerId;
	}
};

void ESPINNER_ID_Callback(Control *sender, int type) {
	// Search if ID is already assigned
	debugCallback(sender, type);

	ESPinner *espinnerInList =
		ESPinner_Manager::getInstance().findESPinnerById(sender->value);
	if (espinnerInList == nullptr) {
		char *backgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(sender->id, backgroundStyle);
	} else {
		char *backgroundStyle = getBackground(DANGER_COLOR);
		ESPUI.setElementStyle(sender->id, backgroundStyle);
	}
}

#include "../mods/ESPinner_DC/ESPinner_DC_Controls.h"
#include "../mods/ESPinner_Encoder/ESPinner_Encoder_Controls.h"
#include "../mods/ESPinner_GPIO/ESPinner_GPIO_Controls.h"
#include "../mods/ESPinner_LCD/ESPinner_LCD_Controls.h"
#include "../mods/ESPinner_MPU/ESPinner_MPU_Controls.h"
#include "../mods/ESPinner_NeoPixel/ESPinner_NeoPixel_Controls.h"
#include "../mods/ESPinner_RFID/ESPinner_RFID_Controls.h"
#include "../mods/ESPinner_Stepper/ESPinner_Stepper_Controls.h"
#include "../mods/ESPinner_TFT/ESPinner_TFT_Controls.h"

#endif