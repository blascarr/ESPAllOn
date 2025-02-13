#ifndef _ESPINNER_LCD_H
#define _ESPINNER_LCD_H

#include "../../controllers/ESPAllOnPinManager.h"
#include <ESPUI.h>

class ESPinner_LCD : public ESPinner {
  public:
	uint8_t gpio;
	ESPinner_LCD(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_LCD() : ESPinner(ESPinner_Mod::LCD) {}
	void setup() override { DUMPSLN("Iniciacion configuración de LCD..."); }
	void update() override { DUMPSLN("Update configuración de LCD..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de LCD...");
	}

	void loader() override { DUMPSLN("Cargando configuración de LCD..."); }
	void setGPIO(uint8_t gpio_pin) { gpio = gpio_pin; }
	uint8_t getGPIO() { return gpio; }
	JsonDocument serializeJSON() override {
		StaticJsonDocument<256> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = "ESPINNER_LCD";
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