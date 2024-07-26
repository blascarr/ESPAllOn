#ifndef _ESPINNER_TFT_H
#define _ESPINNER_TFT_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_TFT : public ESPinner {
  public:
	ESPinner_TFT(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_TFT() : ESPinner(ESPinner_Mod::TFT) {}
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