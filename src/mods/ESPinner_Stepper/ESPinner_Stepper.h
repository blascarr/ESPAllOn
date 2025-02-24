#ifndef _ESPINNER_STEPPER_H
#define _ESPINNER_STEPPER_H

#include "../../controllers/ESPAllOnPinManager.h"
#include <ESPUI.h>

#include <TMCStepper.h>

enum class Stepper_Driver {
	TMC2208,
	TMC2209,
	TMC2130,
	TMC2160,
	TMC2224,
	TMC2660,
	UNKNOWN
};

const std::map<Stepper_Driver, const String> driverName = {
	{Stepper_Driver::TMC2208, "TMC2208"}, {Stepper_Driver::TMC2209, "TMC2209"},
	{Stepper_Driver::TMC2130, "TMC2130"}, {Stepper_Driver::TMC2160, "TMC2160"},
	{Stepper_Driver::TMC2224, "TMC2224"}, {Stepper_Driver::UNKNOWN, "UNKNOWN"}};

class ESPinner_Stepper : public ESPinner {
  public:
	uint8_t gpioA, gpioAA, gpioB, gpioBB;
	Stepper_Driver driver;

	ESPinner_Stepper(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_Stepper() : ESPinner(ESPinner_Mod::Stepper) {}
	void setup() override {
		DUMPSLN("Iniciacion configuración de Stepper ...");
	}
	void update() override { DUMPSLN("Update configuración de Stepper ..."); }
	void implement() override {
		DUMPSLN("Implementacion configuración de Stepper ...");
	}

	void loader() override { DUMPSLN("Cargando configuración de Stepper ..."); }
	void setGPIOA(uint8_t gpio_pin) { gpioA = gpio_pin; }
	uint8_t getGPIOA() { return gpioA; }
	void setGPIOAA(uint8_t gpio_pin) { gpioAA = gpio_pin; }
	uint8_t getGPIOAA() { return gpioAA; }
	void setGPIOB(uint8_t gpio_pin) { gpioB = gpio_pin; }
	uint8_t getGPIOB() { return gpioB; }
	void setGPIOBB(uint8_t gpio_pin) { gpioBB = gpio_pin; }
	uint8_t getGPIOBB() { return gpioBB; }

	const String getPinDriverName(Stepper_Driver driverType) {
		auto it = driverName.find(driverType);
		if (it != driverName.end()) {
			return it->second;
		}
		return "Unknown";
	}

	Stepper_Driver findDriverByName(const String &name) {
		for (const auto &pair : driverName) {
			if (pair.second == name) {
				return pair.first;
			}
		}
		return Stepper_Driver::UNKNOWN;
	}

	void setDriver(Stepper_Driver stepper_driver) { driver = stepper_driver; }
	void setDriver(const String &name) {
		Stepper_Driver foundDriver = findDriverByName(name);
		if (foundDriver != Stepper_Driver::UNKNOWN) {
			driver = foundDriver;
		} else {
			// std::cerr << "Driver not found: " << name << std::endl;
		}
	}
	Stepper_Driver getDriver() { return driver; }
	String get_driver() { return getPinDriverName(driver); }

	JsonDocument serializeJSON() override {
		StaticJsonDocument<256> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = ESPINNER_STEPPER_JSONCONFIG;
		doc[ESPINNER_ID_JSONCONFIG] = ESPinner::getID();
		doc[ESPINNER_STEPPER_A_CONFIG] = getGPIOA();
		doc[ESPINNER_STEPPER_AA_CONFIG] = getGPIOAA();
		doc[ESPINNER_STEPPER_B_CONFIG] = getGPIOB();
		doc[ESPINNER_STEPPER_BB_CONFIG] = getGPIOBB();
		doc[ESPINNER_STEPPER_DRIVER_CONFIG] = get_driver();
		return doc;
	}

	bool deserializeJSON(const String &data) {
		StaticJsonDocument<256> doc;
		DeserializationError error = deserializeJson(doc, data);
		if (error) {
			return false;
		}
		String ID = doc[ESPINNER_ID_JSONCONFIG].as<const char *>();
		ESPinner::setID(ID);
		uint8_t GPIOA = doc[ESPINNER_STEPPER_A_CONFIG].as<uint8_t>();
		setGPIOA(GPIOA);
		uint8_t GPIOAA = doc[ESPINNER_STEPPER_AA_CONFIG].as<uint8_t>();
		setGPIOAA(GPIOAA);
		uint8_t GPIOB = doc[ESPINNER_STEPPER_B_CONFIG].as<uint8_t>();
		setGPIOB(GPIOB);
		uint8_t GPIOBB = doc[ESPINNER_STEPPER_BB_CONFIG].as<uint8_t>();
		setGPIOBB(GPIOBB);
		String DRIVER = doc[ESPINNER_STEPPER_DRIVER_CONFIG].as<const char *>();
		setDriver(DRIVER);
		return true;
	};
};

#endif