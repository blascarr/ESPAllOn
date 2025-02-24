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
					ESPinners.push_back(std::move(espinner));

					// Get Main Control in Tab
					// espinner->createGUI();
					uint16_t parentRef = getTab(TabType::BasicTab);
					if (mod == ESPINNER_GPIO_JSONCONFIG) {
						// GPIO_UIFromESPinner(parentRef);
					}
					// createGUIFromStorage(mod);
					// Create Model
					// saveElement_callback(tab, B_UP);
				}
			}
			// TODO: Create ESPinners in UI
			//
			// ESPAllOnPinManager::getInstance().attach()
		}
	}

	void push(std::unique_ptr<ESPinner> GUIESPinner) {
		ESPinners.push_back(std::move(GUIESPinner));
		saveESPinnersInStorage();
	}

	void detach(const String &id) {
		DUMPSLN("DETACH IN ESPINNER MANAGER");
		auto it =
			std::find_if(ESPinners.begin(), ESPinners.end(),
						 [&id](const std::unique_ptr<ESPinner> &espinner) {
							 return espinner->getID() == id;
						 });

		if (it != ESPinners.end()) {
			ESPinners.erase(it);
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
	// void clearStorage() { ESPinnerManager.getStorageModel()->removeAll(); }
};

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