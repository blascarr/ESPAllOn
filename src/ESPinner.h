#ifndef _ESPINNER_H
#define _ESPINNER_H

#include "ESP_Boards.h"
#include "PinManager.h"
#include <ESPUI.h>

class ESPinner {
  public:
	/*ESPinner(PinManager<ESP_BoardConf, PinMode> *pinManager)
		: pinManager(pinManager) {}*/
	ESPinner() {}
	// Inicialización
	virtual void setup() = 0;
	// Updates
	virtual void update() = 0;
	// Accion de carga del modelo en interfaz
	virtual void loader() = 0;

	// implementacion
	virtual void implement() = 0;
	virtual ~ESPinner() {}
	ESP_PinMode *pinManager;
};

#include "mods/ESPinner_DC.h"
#include "mods/ESPinner_GPIO.h"
#include "mods/ESPinner_LCD.h"
#include "mods/ESPinner_MPU.h"
#include "mods/ESPinner_NeoPixel.h"
#include "mods/ESPinner_Stepper.h"
#include "mods/ESPinner_TFT.h"
#endif