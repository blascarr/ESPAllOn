#ifndef _ESPINNER_LCD_H
#define _ESPINNER_LCD_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_LCD : public ESPinner {
  public:
	ESPinner_LCD(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_LCD() : ESPinner(ESPinner_Mod::LCD) {}
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