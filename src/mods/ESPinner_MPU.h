#ifndef _ESPINNER_MPU_H
#define _ESPINNER_MPU_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_MPU : public ESPinner {
  public:
	ESPinner_MPU(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_MPU() : ESPinner(ESPinner_Mod::MPU) {}
	void setup() override {
		DUMPSLN("Iniciacion configuración de GPIO...");
		// pinManager->isPinOK(12);
	}
	void update() override { DUMPSLN("Update configuración de GPIO..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de GPIO...");
	}

	void loader() override { DUMPSLN("Cargando configuración de GPIO..."); }
};

#endif