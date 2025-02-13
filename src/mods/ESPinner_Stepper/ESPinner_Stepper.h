#ifndef _ESPINNER_STEPPER_H
#define _ESPINNER_STEPPER_H

#include "../../controllers/ESPAllOnPinManager.h"
#include <ESPUI.h>

class ESPinner_Stepper : public ESPinner {
  public:
	uint8_t gpio;
	ESPinner_Stepper(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_Stepper() : ESPinner(ESPinner_Mod::Stepper) {}
	void setup() override {
		DUMPSLN("Iniciacion configuración de Stepper ...");
	}
	void update() override { DUMPSLN("Update configuración de Stepper ..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de Stepper ...");
	}

	void loader() override { DUMPSLN("Cargando configuración de Stepper ..."); }
	void setGPIO(uint8_t gpio_pin) { gpio = gpio_pin; }
	uint8_t getGPIO() { return gpio; }
	JsonDocument serializeJSON() override {
		StaticJsonDocument<256> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = "ESPINNER_STEPPER";
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