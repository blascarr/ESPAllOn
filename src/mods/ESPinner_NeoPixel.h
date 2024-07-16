#ifndef _ESPINNER_NEOPIXEL_H
#define _ESPINNER_NEOPIXEL_H

#include "../ESPinner.h"
#include <ESPUI.h>

class ESPinner_NEOPIXEL : public ESPinner {
  public:
	void setup() override {
		Serial.print("Iniciacion configuración de GPIO...");
		// pinManager->isPinOK(12);
	}
	void update() override { Serial.print("Update configuración de GPIO..."); }
	void implement() override {
		Serial.print("Implementacion configuración de GPIO...");
	}

	void loader() override {
		Serial.print("Cargando configuración de GPIO...");
	}
};

#endif