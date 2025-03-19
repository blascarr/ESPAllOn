#ifndef _ESPINNER_DC_H
#define _ESPINNER_DC_H

#include "../../controllers/ESPAllOnPinManager.h"
#include <ESPUI.h>

enum class DCPin { PinA, PinB };
class ESPinner_DC : public ESPinner {
  public:
	uint8_t gpioA, gpioB;
	bool direction;
	bool running;
	uint8_t speed;

	ESPinner_DC(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_DC() : ESPinner(ESPinner_Mod::DC) {}

	void setup() override { DUMPSLN("Iniciacion configuración de DC..."); }
	void update() override { DUMPSLN("Update configuración de DC..."); }
	void implement() override;

	void loader() override { DUMPSLN("Cargando configuración de DC..."); }

	void setDirection(bool dir) { direction = dir; }
	void setRunning(bool run) { running = run; }
	void setSpeed(int spd) { speed = constrain(spd, 0, 255); }

	uint8_t getGPIO(DCPin pinType) {
		if (pinType == DCPin::PinA) {
			return gpioA;
		}
		if (pinType == DCPin::PinB) {
			return gpioB;
		}
		return 0; // GPIO 0 should not be used, but configured in final return
	}

	void setGPIOA(uint8_t gpio_Apin) { gpioA = gpio_Apin; }
	uint8_t getGPIOA() { return getGPIO(DCPin::PinA); }
	void setGPIOB(uint8_t gpio_Bpin) { gpioB = gpio_Bpin; }
	uint8_t getGPIOB() { return getGPIO(DCPin::PinB); }

	ESP_PinMode getPinModeConf(DCPin pinType) {
		ESP_PinMode pinMode = {getGPIO(pinType), OutputPin(true),
							   PinType::BusPWM};
		return pinMode;
	};

	JsonDocument serializeJSON() override {
		StaticJsonDocument<256> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = ESPINNER_DC_JSONCONFIG;
		doc[ESPINNER_ID_JSONCONFIG] = ESPinner::getID();
		doc[ESPINNER_DCA_JSONCONFIG] = getGPIOA();
		doc[ESPINNER_DCB_JSONCONFIG] = getGPIOB();
		return doc;
	}

	bool deserializeJSON(const String &data) {
		StaticJsonDocument<256> doc;
		DeserializationError error = deserializeJson(doc, data);
		if (error) {
			return false;
		}
		String ID = doc[ESPINNER_ID_JSONCONFIG].as<const char *>();
		ESPinner_DC::setID(ID);
		Serial.print(doc[ESPINNER_ID_JSONCONFIG].as<const char *>());
		Serial.print(doc[ESPINNER_DCA_JSONCONFIG].as<int>());
		Serial.print(doc[ESPINNER_DCB_JSONCONFIG].as<int>());
		ESPinner_DC::setGPIOA(doc[ESPINNER_DCA_JSONCONFIG].as<int>());
		ESPinner_DC::setGPIOB(doc[ESPINNER_DCB_JSONCONFIG].as<int>());

		return true;
	};
};

void createDCMod_callback(Control *sender, int type);
void saveButtonDCCheck(uint16_t parentRef);
void saveDC_callback(Control *sender, int type);
void removeDC_callback(Control *sender, int type);
void DCSelector_callback(Control *sender, int type);
void DC_Selector(uint16_t PIN_ptr);
void DC_UI(uint16_t DC_ptr);

#endif