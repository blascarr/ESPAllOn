#ifndef _ESPINNER_STEPPER_H
#define _ESPINNER_STEPPER_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_Stepper : public ESPinner {
  public:
	void setup() override {
		DUMPSLN("Iniciacion configuración de Stepper ...");
	}
	void update() override { DUMPSLN("Update configuración de Stepper ..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de Stepper ...");
	}

	void loader() override { DUMPSLN("Cargando configuración de Stepper ..."); }
};

#endif