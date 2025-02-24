#ifndef _ESPINNER_NEOPIXEL_H
#define _ESPINNER_NEOPIXEL_H

#include "../../controllers/ESPAllOnPinManager.h"
#include <ESPUI.h>

class ESPinner_NEOPIXEL : public ESPinner {
  public:
	uint8_t gpio;
	ESPinner_NEOPIXEL(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_NEOPIXEL() : ESPinner(ESPinner_Mod::NeoPixel) {}
	void setup() override {
		DUMPSLN("Iniciacion configuración de NEOPIXEL...");
	}
	void update() override { DUMPSLN("Update configuración de NEOPIXEL..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de NEOPIXEL...");
	}

	void loader() override { DUMPSLN("Cargando configuración de NEOPIXEL..."); }
	void setGPIO(uint8_t gpio_pin) { gpio = gpio_pin; }
	uint8_t getGPIO() { return gpio; }
	JsonDocument serializeJSON() override {
		StaticJsonDocument<256> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = ESPINNER_NEOPIXEL_JSONCONFIG;
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