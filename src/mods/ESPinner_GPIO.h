#ifndef _ESPINNER_GPIO_H
#define _ESPINNER_GPIO_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_GPIO : public ESPinner {
  public:
	void init() override {
		Serial.print("Iniciacion configuración de GPIO...");
	}
	void update() override { Serial.print("Update configuración de GPIO..."); }
	void implement() override {
		Serial.print("Implementacion configuración de GPIO...");
	}

	void loader() override {
		Serial.print("Cargando configuración de GPIO...");
	}

	void save() override { Serial.print("Guardando configuración de GPIO..."); }

	void debug() override { Serial.print("Debugging GPIO..."); }
};

#endif