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
	virtual void init() = 0;
	// Updates
	virtual void update() = 0;
	// Accion de carga del modelo en interfaz
	virtual void loader() = 0;
	// Accuón al hacer click en guardar
	virtual void save() = 0;
	// Gestión de Debug
	virtual void debug() = 0;

	// implementacion
	virtual void implement() = 0;

  private:
	PinManager<ESP_BoardConf, PinMode> *pinManager;
};

#include "mods/ESPinner_GPIO.h"
#include "mods/ESPinner_Stepper.h"
#endif