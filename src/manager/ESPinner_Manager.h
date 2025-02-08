#ifndef _ESPINNER_MANAGER_H
#define _ESPINNER_MANAGER_H

#include "../config.h"
#include "./Storage_Manager.h"
#include <Persistance.h>

#include "../controllers/ESPinner.h"

#include "./upcast/upcast_utils.h"

class ESPinner_Manager {
  private:
	std::list<std::unique_ptr<ESPinner>> ESPinners;
	Persistance ESPinnerManager;

  public:
	ESPinner_Manager() : ESPinnerManager(nullptr, &storage) {}
	static ESPinner_Manager &getInstance() {
		static ESPinner_Manager instance;
		return instance;
	}

	bool deserializeJSON(const String &data) {
		StaticJsonDocument<256> doc;
		DeserializationError error = deserializeJson(doc, data);
		if (error) {
			return false;
		}
		// name = doc["ESPinner_Mod"].as<const char *>();
		// isVirus = doc["virus"].as<bool>();
		// int auxMode = doc["GPIO"].as<int>();

		return true;
	};

	void loadFromStorage() {
		DynamicJsonDocument doc(256);
		String serialized = ESPinnerManager.loadData(ESPinner_File);
		DUMP("Dataloaded: ", serialized);
		DeserializationError error = deserializeJson(doc, serialized);
		if (!error) {
			if (!doc.is<JsonArray>()) {
				DUMPSLN("ERROR: NO JSON ARRAY FORMAT.");
				return;
			}
			JsonArray array = doc.as<JsonArray>();
			for (JsonDocument obj : array) {

				String output;
				String mod = obj["ESPinner_Mod"].as<const char *>();
				DUMPLN("MOD: ", mod);
				if (mod == "ESPINNER_GPIO") {
					// ESPinner_GPIO gpioESPINNER;
					// gpioESPINNER.deserializeJSON(output);
				}
			}
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
		ESPinnerManager.getStorageModel()->save(data, ESPinner_File);
	}
};

#include "../mods/ESPinner_GPIO/ESPinner_GPIO_Controls.h"

#endif