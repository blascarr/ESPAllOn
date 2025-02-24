#ifndef _ESPINNER_DC_H
#define _ESPINNER_DC_H

#include "../../controllers/ESPAllOnPinManager.h"
#include <ESPUI.h>

class ESPinner_DC : public ESPinner {
  public:
	uint8_t gpio;
	ESPinner_DC(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_DC() : ESPinner(ESPinner_Mod::DC) {}

	void setup() override { DUMPSLN("Iniciacion configuración de DC..."); }
	void update() override { DUMPSLN("Update configuración de DC..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de DC...");
	}

	void loader() override { DUMPSLN("Cargando configuración de DC..."); }
	void setGPIO(uint8_t gpio_pin) { gpio = gpio_pin; }
	uint8_t getGPIO() { return gpio; }

	JsonDocument serializeJSON() override {
		StaticJsonDocument<256> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = ESPINNER_DC_JSONCONFIG;
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