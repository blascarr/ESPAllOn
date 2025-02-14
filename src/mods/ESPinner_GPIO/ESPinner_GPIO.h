#ifndef _ESPINNER_GPIO_H
#define _ESPINNER_GPIO_H

#include "../../controllers/ESPAllOnPinManager.h"
#include <ESPUI.h>

struct ESPinner_GPIOMode {
	GPIOMode model;
	String name;
};

const ESPinner_GPIOMode GPIO_mods[] = {
	{GPIOMode::Input, GPIO_ESPINNERINPUT_LABEL},
	{GPIOMode::Output, GPIO_ESPINNEROUTPUT_LABEL}};
class ESPinner_GPIO : public ESPinner {
  public:
	uint8_t gpio;
	GPIOMode GPIO_mode;

	ESPinner_GPIO(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_GPIO() : ESPinner(ESPinner_Mod::GPIO) {}
	void setup() override { DUMPSLN("Iniciacion configuración de GPIO..."); }
	void update() override { DUMPSLN("Update configuración de GPIO..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de GPIO...");
	}

	void loader() override { DUMPSLN("Cargando configuración de GPIO..."); }

	void setGPIO(uint8_t gpio_pin) { gpio = gpio_pin; }
	uint8_t getGPIO() { return gpio; }
	void setGPIOMode(GPIOMode mode) { GPIO_mode = mode; }
	void setGPIOMode(String mode) {
		if (mode == ESPINNER_INPUT_CONFIG) {
			setGPIOMode(GPIOMode::Input);
		}
		if (mode == ESPINNER_OUTPUT_CONFIG) {
			setGPIOMode(GPIOMode::Output);
		}
	}

	GPIOMode getGPIOMode() { return GPIO_mode; }

	String getGPIOMode_JSON() {
		if (GPIO_mode == GPIOMode::Input) {
			return ESPINNER_INPUT_CONFIG;
		}
		if (GPIO_mode == GPIOMode::Output) {
			return ESPINNER_OUTPUT_CONFIG;
		}
	}

	ESP_PinMode getPinModeConf() {

		switch (GPIO_mode) {
		case GPIOMode::Input: {
			InputPin model = InputPin(false, false);
			ESP_PinMode pinMode = {this->getGPIO(), model, PinType::BusDigital};
			return pinMode;
		}
		case GPIOMode::Output: {
			OutputPin model = OutputPin(false);
			ESP_PinMode pinMode = {this->getGPIO(), model, PinType::BusDigital};
			return pinMode;
		}
		// If no GPIO_Mode is configured, return as Output by default
		default:
			ESP_PinMode pinMode = {this->getGPIO(), OutputPin(false),
								   PinType::BusDigital};
			return pinMode;
		}
	};
	JsonDocument serializeJSON() override {
		StaticJsonDocument<256> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = "ESPINNER_GPIO";
		doc[ESPINNER_ID_JSONCONFIG] = ESPinner::getID();
		doc[ESPINNER_GPIO_JSONCONFIG] = getGPIO();
		doc[ESPINNER_IO_JSONCONFIG] = getGPIOMode_JSON();

		// doc["Config"] = getPinModeConf();
		/*
		doc["isBroken"] = "ESPINNER_GPIO";
		doc["canDeepSleep"] = "ESPINNER_GPIO";
		doc["isTouchGPIO"] = "ESPINNER_GPIO";
		doc["Config"] = "ESPINNER_GPIO";
		doc["isADC"] = "ESPINNER_GPIO";
		*/
		return doc;
	}
	bool deserializeJSON(const String &data) {
		StaticJsonDocument<256> doc;
		DeserializationError error = deserializeJson(doc, data);
		if (error) {
			return false;
		}
		gpio = doc[ESPINNER_GPIO_JSONCONFIG].as<int>();
		String ID = doc[ESPINNER_ID_JSONCONFIG].as<const char *>();
		ESPinner::setID(ID);
		String GPIOMODE = doc[ESPINNER_IO_JSONCONFIG].as<const char *>();
		setGPIOMode(GPIOMODE);

		return true;
	};
};

#endif