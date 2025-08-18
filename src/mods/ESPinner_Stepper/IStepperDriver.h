#ifndef _IESPINNER_STEPPER_H
#define _IESPINNER_STEPPER_H

#include <Arduino.h>
#include <map>

#include "A4988.h"
#include <AccelStepper.h>
#include <TMCStepper.h>

enum class Stepper_Driver {
	TMC2208,
	TMC2209,
	TMC2130,
	TMC2160,
	TMC2660,
	TMC5130,
	TMC5160,
	A4988,
	ACCELSTEPPER,
	UNKNOWN
};

const std::map<Stepper_Driver, const String> driverName = {
	{Stepper_Driver::TMC2208, "TMC2208"},
	{Stepper_Driver::TMC2209, "TMC2209"},
	{Stepper_Driver::TMC2130, "TMC2130"},
	{Stepper_Driver::TMC2160, "TMC2160"},
	{Stepper_Driver::TMC2660, "TMC2660"},
	{Stepper_Driver::TMC5130, "TMC5130"},
	{Stepper_Driver::TMC5160, "TMC5160"},
	{Stepper_Driver::A4988, "A4988"},
	{Stepper_Driver::ACCELSTEPPER, "ACCELSTEPPER"},
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

struct IStepperDriver {
	virtual ~IStepperDriver() = default;
	virtual void begin() = 0;
	virtual void enable(bool on) = 0;
	virtual void setMicrosteps(uint16_t ms) {}
	virtual void setRMSCurrent(uint16_t mA) {}
};

// ===================== Adapter A4988 =====================
class A4988Adapter : public IStepperDriver {
  public:
	A4988Adapter(uint8_t dirPin, uint8_t stepPin, uint8_t enPin)
		: stepper(200, dirPin, stepPin, enPin), en(enPin) {}

	A4988Adapter(uint8_t dirPin, uint8_t stepPin, uint8_t enPin,
				 int stepsPerRev)
		: stepper(stepsPerRev, dirPin, stepPin, enPin), en(enPin) {}

	A4988Adapter(int stepsPerRev, uint8_t dirPin, uint8_t stepPin,
				 uint8_t sleepPin, int rpm, int microsteps)
		: spr(stepsPerRev), rpm(rpm), microsteps(microsteps),
		  stepper(stepsPerRev, dirPin, stepPin), sleep(sleepPin) {}

	A4988Adapter(int stepsPerRev, uint8_t dirPin, uint8_t stepPin,
				 uint8_t ms1Pin, uint8_t ms2Pin, uint8_t ms3Pin,
				 uint8_t sleepPin, int rpm, int microsteps)
		: spr(stepsPerRev), rpm(rpm), microsteps(microsteps),
		  stepper(stepsPerRev, dirPin, stepPin, ms1Pin, ms2Pin, ms3Pin),
		  sleep(sleepPin) {}
	void begin() override {
		pinMode(en, OUTPUT);
		digitalWrite(en, LOW);
	}
	void enable(bool on) override { digitalWrite(sleep, on ? HIGH : LOW); }

	void setMicrosteps(uint16_t ms) override {
		microsteps = ms;
		stepper.setMicrostep(ms);
	}

	void setRMSCurrent(uint16_t /*mA*/) override {
		// A4988 does not support RMS current by software, it is manual
	}

	void rotate(int revs) { stepper.rotate(revs); }

  private:
	int spr;		// steps per revolution
	int rpm;		// revolutions per minute
	int microsteps; // microsteps per step
	uint8_t sleep;	// sleep pin
	uint8_t en;		// enable pin
	A4988 stepper;
};

// ===================== Adapter AccelStepper =====================
class AccelStepperAdapter : public IStepperDriver {
  public:
	AccelStepperAdapter(uint8_t dirPin, uint8_t stepPin, uint8_t enPin = 0)
		: stepper(AccelStepper::DRIVER, dirPin, stepPin), en(enPin) {}

  private:
	int spr;		// steps per revolution
	int rpm;		// revolutions per minute
	int microsteps; // microsteps per step
	uint8_t sleep;	// sleep pin
	uint8_t en;		// enable pin
	AccelStepper stepper;
};

// ===================== Adapter TMC2130 =====================
class TMC2130Adapter : public IStepperDriver {
  public:
	TMC2130Adapter(uint8_t csPin, uint8_t enPin, float rSense,
				   uint16_t iRMSmA = 900, uint16_t micro = 16)
		: cs(csPin), en(enPin), r_sense(rSense), current(iRMSmA),
		  microsteps(micro), drv(csPin, rSense) {}

	void begin() override {
		pinMode(en, OUTPUT);
		digitalWrite(en, HIGH);
		drv.begin(); // Initialize SPI and driver
		drv.toff(5); // Obligatory >0 to enable chopper
		drv.blank_time(24);
		drv.rms_current(current);
		drv.microsteps(microsteps);
		drv.en_pwm_mode(true); // (optional) stealthChop
		digitalWrite(en, LOW); // Enable outputs
	}

	void enable(bool on) override { digitalWrite(en, on ? LOW : HIGH); }
	void setMicrosteps(uint16_t ms) override { drv.microsteps(ms); }
	void setRMSCurrent(uint16_t mA) override { drv.rms_current(mA); }

  private:
	uint8_t cs, en;
	float r_sense = 0.11f;
	uint16_t current = 900;
	uint16_t microsteps = 16;
	TMC2130Stepper drv;
};
#endif