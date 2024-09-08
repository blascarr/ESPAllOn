#ifndef _ESPINNER_H
#define _ESPINNER_H

#include "PinManager.h"
#include <ESPUI.h>

enum class ESPinner_Mod {
	VOID,
	GPIO,
	Stepper,
	RFID,
	NeoPixel,
	DC,
	MPU,
	Encoder,
	TFT,
	LCD
};

struct ESPinner_Module {
	ESPinner_Mod model;
	String name;
};

const ESPinner_Module mods[] = {{ESPinner_Mod::VOID, VOID_LABEL},
								{ESPinner_Mod::GPIO, GPIO_LABEL},
								{ESPinner_Mod::Stepper, STEPPER_LABEL},
								{ESPinner_Mod::RFID, RFID_LABEL},
								{ESPinner_Mod::NeoPixel, NEOPIXEL_LABEL},
								{ESPinner_Mod::DC, DC_LABEL},
								{ESPinner_Mod::MPU, MPU_LABEL},
								{ESPinner_Mod::Encoder, ENCODER_LABEL},
								{ESPinner_Mod::TFT, TFT_LABEL},
								{ESPinner_Mod::LCD, LCD_LABEL}};

class IESPinner {
  public:
	IESPinner() {}
	// Inicialización
	virtual void setup() = 0;
	// Updates
	virtual void update() = 0;
	// Accion de carga del modelo en interfaz
	virtual void loader() = 0;

	// implementacion
	virtual void implement() = 0;
	virtual ~IESPinner() {}
};

class ESPinner : public IESPinner {
  public:
	ESPinner_Mod mod;
	String ID;

	ESPinner(ESPinner_Mod espinner_mod) : mod(espinner_mod) {};
	ESPinner() : mod(ESPinner_Mod::VOID) {}
	void setup() override {}
	void update() override {}
	void implement() override {}
	void loader() override {}

	void setType(ESPinner_Mod espinner_mod) { mod = espinner_mod; }
	ESPinner_Mod getType() { return mod; }
	void setID(String espinner_ID) { ID = espinner_ID; }
	String getID() { return ID; }
};

#include "mods/ESPinner_DC.h"
#include "mods/ESPinner_Encoder.h"
#include "mods/ESPinner_GPIO.h"
#include "mods/ESPinner_LCD.h"
#include "mods/ESPinner_MPU.h"
#include "mods/ESPinner_NeoPixel.h"
#include "mods/ESPinner_RFID.h"
#include "mods/ESPinner_Stepper.h"
#include "mods/ESPinner_TFT.h"
#endif