#ifndef _ESPINNER_RFID_H
#define _ESPINNER_RFID_H

#include "../../controllers/ESPAllOnPinManager.h"
#include <ESPUI.h>

class ESPinner_RFID : public ESPinner {
  public:
	uint8_t gpio;
	ESPinner_RFID(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_RFID() : ESPinner(ESPinner_Mod::RFID) {}
	void setup() override { DUMPSLN("Iniciacion configuración de RFID..."); }
	void update() override { DUMPSLN("Update configuración de RFID..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de RFID...");
	}

	void loader() override { DUMPSLN("Cargando configuración de RFID..."); }
	void setGPIO(uint8_t gpio_pin) { gpio = gpio_pin; }
	uint8_t getGPIO() { return gpio; }
	JsonDocument serializeJSON() override {
		StaticJsonDocument<256> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = ESPINNER_RFID_JSONCONFIG;
		doc[ESPINNER_ID_JSONCONFIG] = ESPinner::getID();
		doc[ESPINNER_GPIO_JSONCONFIG] = getGPIO();
		return doc;
	}

	bool deserializeJSON(const String &data) {
		StaticJsonDocument<256> doc;
		DeserializationError error = deserializeJson(doc, data);
		if (error) {
			return false;
		}
		return true;
	};
};

#endif