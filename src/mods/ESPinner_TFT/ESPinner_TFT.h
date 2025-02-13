#ifndef _ESPINNER_TFT_H
#define _ESPINNER_TFT_H

#include "../../controllers/ESPAllOnPinManager.h"
#include <ESPUI.h>

class ESPinner_TFT : public ESPinner {
  public:
	uint8_t gpio;
	ESPinner_TFT(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_TFT() : ESPinner(ESPinner_Mod::TFT) {}
	void setup() override { DUMPSLN("Iniciacion configuración de TFT..."); }
	void update() override { DUMPSLN("Update configuración de TFT..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de TFT...");
	}

	void loader() override { DUMPSLN("Cargando configuración de TFT..."); }
	void setGPIO(uint8_t gpio_pin) { gpio = gpio_pin; }
	uint8_t getGPIO() { return gpio; }
	JsonDocument serializeJSON() override {
		StaticJsonDocument<256> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = "ESPINNER_TFT";
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