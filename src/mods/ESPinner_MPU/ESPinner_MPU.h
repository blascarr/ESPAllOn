#ifndef _ESPINNER_MPU_H
#define _ESPINNER_MPU_H

#include "../../controllers/ESPAllOnPinManager.h"
#include <ESPUI.h>

class ESPinner_MPU : public ESPinner {
  public:
	uint8_t gpio;
	ESPinner_MPU(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_MPU() : ESPinner(ESPinner_Mod::MPU) {}
	void setup() override { DUMPSLN("Iniciacion configuración de MPU..."); }
	void update() override { DUMPSLN("Update configuración de MPU..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de MPU...");
	}

	void loader() override { DUMPSLN("Cargando configuración de MPU..."); }
	void setGPIO(uint8_t gpio_pin) { gpio = gpio_pin; }
	uint8_t getGPIO() { return gpio; }
	JsonDocument serializeJSON() override {
		StaticJsonDocument<256> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = ESPINNER_MPU_JSONCONFIG;
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