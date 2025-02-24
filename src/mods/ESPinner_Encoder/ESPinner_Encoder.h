#ifndef _ESPINNER_ENCODER_H
#define _ESPINNER_ENCODER_H

#include "../../controllers/ESPAllOnPinManager.h"
#include <ESPUI.h>

class ESPinner_Encoder : public ESPinner {
  public:
	uint8_t gpio;
	ESPinner_Encoder(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_Encoder() : ESPinner(ESPinner_Mod::Encoder) {}
	void setup() override { DUMPSLN("Iniciacion configuración de ENCODER..."); }
	void update() override { DUMPSLN("Update configuración de ENCODER..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de ENCODER...");
	}

	void loader() override { DUMPSLN("Cargando configuración de ENCODER..."); }
	void setGPIO(uint8_t gpio_pin) { gpio = gpio_pin; }
	uint8_t getGPIO() { return gpio; }
	JsonDocument serializeJSON() override {
		StaticJsonDocument<256> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = ESPINNER_ENCODER_JSONCONFIG;
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