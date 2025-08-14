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
	A4988,
	UNKNOWN
};

const std::map<Stepper_Driver, const String> driverName = {
	{Stepper_Driver::TMC2208, "TMC2208"}, {Stepper_Driver::TMC2209, "TMC2209"},
	{Stepper_Driver::TMC2130, "TMC2130"}, {Stepper_Driver::TMC2160, "TMC2160"},
	{Stepper_Driver::TMC2224, "TMC2224"}, {Stepper_Driver::A4988, "A4988"},
	{Stepper_Driver::UNKNOWN, "UNKNOWN"}};

enum class StepperPin { STEP, DIR, EN, CS, DIAG0, DIAG1 };

String getDriverName(Stepper_Driver driverType) {
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
class ESPinner_Stepper : public ESPinner {
  public:
	uint8_t DIR, STEP, EN;
	uint8_t CS, DIAG0, DIAG1;
	bool isSPI = false;
	bool isDIAG = false;
	Stepper_Driver driver = Stepper_Driver::TMC2130;
	std::unique_ptr<TMCStepper> tmc;

	ESPinner_Stepper(ESPinner_Mod espinner_mod) : ESPinner(espinner_mod) {}
	ESPinner_Stepper() : ESPinner(ESPinner_Mod::Stepper) {}
	void setup() override {
		DUMPSLN("Iniciacion configuración de Stepper ...");
	}
	void update() override { DUMPSLN("Update configuración de Stepper ..."); }
	void loader() override { DUMPSLN("Cargando configuración de Stepper ..."); }
	void implement() override;

	uint8_t getGPIO(StepperPin pinType) {
		if (pinType == StepperPin::STEP) {
			return STEP;
		}
		if (pinType == StepperPin::DIR) {
			return DIR;
		}
		if (pinType == StepperPin::EN) {
			return EN;
		}
		if (pinType == StepperPin::CS) {
			return CS;
		}
		if (pinType == StepperPin::DIAG0) {
			return DIAG0;
		}
		if (pinType == StepperPin::DIAG1) {
			return DIAG1;
		}
		return 0; // GPIO 0 should not be used, but configured in final return
	}

	void setDriver(Stepper_Driver StepperDriver) { driver = StepperDriver; }
	void setDriver(const String &name) {
		Stepper_Driver foundDriver = findDriverByName(name);
		if (foundDriver != Stepper_Driver::UNKNOWN) {
			driver = foundDriver;
		} else {
			// std::cerr << "Driver not found: " << name << std::endl;
		}
	}

	Stepper_Driver getDriver() { return driver; }
	String get_driverName() { return getDriverName(getDriver()); }

	void setDIR(uint8_t dir) { DIR = dir; }
	uint8_t getDIR() { return DIR; }
	void setSTEP(uint8_t step) { STEP = step; }
	uint8_t getSTEP() { return STEP; }
	void setEN(uint8_t en) { EN = en; }
	uint8_t getEN() { return EN; }
	void setCS(uint8_t cs) { CS = cs; }
	uint8_t getCS() { return CS; }
	void setDIAG0(uint8_t diag0) { DIAG0 = diag0; }
	uint8_t getDIAG0() { return DIAG0; }
	void setDIAG1(uint8_t diag1) { DIAG1 = diag1; }
	uint8_t getDIAG1() { return DIAG1; }

	void setSPI(bool isspi) { isSPI = isspi; }
	uint8_t getSPI() { return isSPI; }

	void setISDIAG(bool isdiag) { isDIAG = isdiag; }
	uint8_t getISDIAG() { return isDIAG; }

	ESP_PinMode getPinModeConf(StepperPin pinType) {
		ESP_PinMode pinMode = {getGPIO(pinType), OutputPin(true),
							   PinType::BusPWM};
		return pinMode;
	};

	JsonDocument serializeJSON() override {
		StaticJsonDocument<256> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = ESPINNER_STEPPER_JSONCONFIG;
		doc[ESPINNER_ID_JSONCONFIG] = ESPinner::getID();
		doc[ESPINNER_STEPPER_STEP_CONFIG] = getSTEP();
		doc[ESPINNER_STEPPER_DIR_CONFIG] = getDIR();
		doc[ESPINNER_STEPPER_EN_CONFIG] = getEN();
		doc[ESPINNER_STEPPER_DRIVER_CONFIG] = get_driverName();
		if (getSPI()) {
			doc[ESPINNER_STEPPER_ISSPI_CONFIG] = getSPI();
			doc[ESPINNER_STEPPER_CS_CONFIG] = getCS();
		}

		if (getISDIAG()) {
			doc[ESPINNER_STEPPER_DIAG0_CONFIG] = getDIAG0();
			doc[ESPINNER_STEPPER_DIAG1_CONFIG] = getDIAG1();
			doc[ESPINNER_STEPPER_ISDIAG_CONFIG] = getISDIAG();
		}
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
		uint8_t _STEP = doc[ESPINNER_STEPPER_STEP_CONFIG].as<uint8_t>();
		setSTEP(_STEP);
		uint8_t _DIR = doc[ESPINNER_STEPPER_DIR_CONFIG].as<uint8_t>();
		setDIR(_DIR);
		uint8_t _EN = doc[ESPINNER_STEPPER_EN_CONFIG].as<uint8_t>();
		setEN(_EN);

		bool isspi = doc[ESPINNER_STEPPER_ISSPI_CONFIG].as<bool>();
		if (isspi) {
			uint8_t _CS = doc[ESPINNER_STEPPER_CS_CONFIG].as<uint8_t>();
			setCS(_CS);
		}

		bool isdiag = doc[ESPINNER_STEPPER_ISDIAG_CONFIG].as<bool>();
		if (isdiag) {
			uint8_t _DIAG0 = doc[ESPINNER_STEPPER_DIAG0_CONFIG].as<uint8_t>();
			setDIAG0(_DIAG0);
			uint8_t _DIAG1 = doc[ESPINNER_STEPPER_DIAG1_CONFIG].as<uint8_t>();
			setDIAG1(_DIAG1);
		}
		String DRIVER = doc[ESPINNER_STEPPER_DRIVER_CONFIG].as<const char *>();
		setDriver(DRIVER);
		return true;
	};
};

void createStepper_callback(Control *sender, int type);
void saveStepper_callback(Control *sender, int type);
void removeStepper_callback(Control *sender, int type);
void StepperSelector_callback(Control *sender, int type);
// void createStepper_callback(Control *sender, int type);
void Stepper_Selector(uint16_t PIN_ptr);
void Stepper_UI(uint16_t GPIO_ptr);

#endif