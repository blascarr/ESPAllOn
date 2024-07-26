#ifndef _ESPINNER_NEOPIXEL_H
#define _ESPINNER_NEOPIXEL_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_NEOPIXEL : public ESPinner {
  public:
	ESPinner_NEOPIXEL(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_NEOPIXEL() : ESPinner(ESPinner_Mod::NeoPixel) {}
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