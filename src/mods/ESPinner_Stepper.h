#ifndef _ESPINNER_STEPPER_H
#define _ESPINNER_STEPPER_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_Stepper : public ESPinner {
  public:
	void setup() override {
		Serial.print("Iniciacion configuración de Stepper ...");
	}
	void update() override {
		Serial.print("Update configuración de Stepper ...");
	}
	void implement() override {
		Serial.print("Implementacion configuración de Stepper ...");
	}

	void loader() override {
		Serial.print("Cargando configuración de Stepper ...");
	}
};

#endif