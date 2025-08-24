#ifndef _ESPINNER_STEPPER_H
#define _ESPINNER_STEPPER_H

#include "../../controllers/ESPAllOnPinManager.h"

#include <ESPUI.h>

#include "./IStepperDriver.h"
#include "./MotionController.h"

class ESPinner_Stepper : public ESPinner {
  public:
	uint8_t DIR, STEP, EN;
	uint8_t CS, DIAG0, DIAG1;
	uint8_t R_SENSE = 0.11f;

	bool isSPI = false;
	bool isDIAG = false;
	Stepper_Driver driver = Stepper_Driver::UNKNOWN;
	std::unique_ptr<IStepperDriver> stepper;
	std::unique_ptr<AccelStepper> motion;
	std::unique_ptr<MotionController> controller;

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
			setDriver(foundDriver);
			if (driver == Stepper_Driver::TMC2130) {
				// stepper = std::make_unique<TMC2130Adapter>(CS, R_SENSE);
				DUMPSLN("TMC2130 Stepper created");
			}
			if (driver == Stepper_Driver::TMC2208) {
				// TODO: RX, TX by serial Port
				uint8_t RX = 2;
				uint8_t TX = 3;
				// stepper = std::make_unique<TMC2208Stepper>(RX, TX, R_SENSE);
			}
			if (driver == Stepper_Driver::TMC2209) {
				// TODO: RX, TX by serial Port
				uint8_t RX = 2;
				uint8_t TX = 3;
				uint8_t DRIVER_ADDRESS = 0b00;
				// stepper = std::make_unique<TMC2209Stepper>(RX, TX,
				// R_SENSE,DRIVER_ADDRESS);
			}
			if (driver == Stepper_Driver::TMC2160) {
				// stepper = std::make_unique<TMC2160Stepper>(CS, R_SENSE);
			}
			if (driver == Stepper_Driver::TMC2660) {
				// stepper = std::make_unique<TMC2660Stepper>(CS);
				// stepper = std::make_unique<TMC2660Stepper>CS_PIN, R_SENSE,
				// SW_MOSI, SW_MISO, SW_SCK);
			}
			if (driver == Stepper_Driver::TMC5130) {
				// stepper = std::make_unique<TMC5130Stepper>(CS, R_SENSE);
			}
			if (driver == Stepper_Driver::TMC5160) {
				// stepper = std::make_unique<TMC5160Stepper>(CS, R_SENSE);
			}
			if (driver == Stepper_Driver::A4988) {
				// stepper = std::make_unique<A4988Adapter>(DIR, STEP, EN);
				stepper->begin();
			}
			if (driver == Stepper_Driver::ACCELSTEPPER) {
				stepper = std::make_unique<AccelStepperAdapter>(DIR, STEP);
				stepper->begin();
			}
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