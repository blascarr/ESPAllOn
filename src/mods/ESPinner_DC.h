#ifndef _ESPINNER_DC_H
#define _ESPINNER_DC_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_DC : public ESPinner {
  public:
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