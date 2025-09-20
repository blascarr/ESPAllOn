#ifndef _IESPINNER_STEPPER_H
#define _IESPINNER_STEPPER_H

#include "StepperRunner.h"
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
	virtual void enable(bool on) { _active = on; }
	virtual bool isEnabled() const { return _active; }
	virtual void setMicrosteps(uint16_t ms) {}
	virtual void setRMSCurrent(uint16_t mA) {}

	/**
	 * Get AccelStepper instance for registration with StepperRunner
	 * @return Pointer to AccelStepper instance or nullptr if not available
	 */
	virtual AccelStepper *getAccelStepper() { return nullptr; }

	/**
	 * Register this stepper with the global StepperRunner
	 * Should be called after begin() to ensure proper initialization
	 */
	virtual bool registerRunner(const String &id) { return false; }

	/**
	 * Unregister this stepper from the global StepperRunner
	 */
	virtual bool unregisterRunner(const String &id) {
		return StepperRunner::getInstance().unregisterRunnable(id);
	}

  private:
	bool _active = false;
};

// ===================== Adapter A4988 =====================
/*
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

	void setRMSCurrent(uint16_t mA) override {
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
*/
// ===================== Adapter AccelStepper =====================

/* Stepper ESPinner class only used for hardware instantiation */
class AccelStepperAdapter : public IStepperDriver, IRunnable {
  public:
	AccelStepperAdapter(uint8_t stepPin, uint8_t dirPin, uint8_t enPin = 0)
		: stepper(AccelStepper::DRIVER, stepPin, dirPin), step(stepPin),
		  dir(dirPin), en(enPin) {
		this->begin();
	}

	/**
	 * Initialize the AccelStepper driver
	 */
	void begin() override {
		if (en != 0) {
			pinMode(en, OUTPUT);
			digitalWrite(en, LOW); // Enable stepper
		}

		if (dir != 0) {
			pinMode(dir, OUTPUT);
			digitalWrite(dir, LOW);
		}

		stepper.setMaxSpeed(1000);	  // Default max speed
		stepper.setAcceleration(500); // Default acceleration

		// Enable the stepper after initialization
		enable(true);
	}

	void run() override {
		if (isEnabled()) {
			stepper.run();
		}
	}

	bool isActive() const override { return IStepperDriver::isEnabled(); }

	/**
	 * Get the ID of the stepper
	 * @return ID of the stepper
	 */
	String getID() const override { return _id; }

	/**
	 * Get the driver name of the stepper
	 * @return Driver name of the stepper
	 */
	String getDriverName() const override { return "ACCELSTEPPER"; }

	/**
	 * Enable or disable the stepper motor
	 * @param on True to enable, false to disable
	 */
	void enable(bool on) override {
		IStepperDriver::enable(on); // Update _active in base class
		if (en != 0) {
			digitalWrite(en, on ? LOW : HIGH); // LOW = enabled for most drivers
		}
	}

	/**
	 * Get AccelStepper instance for direct access
	 */
	AccelStepper *getAccelStepper() override { return &stepper; }

	/**
	 * Register this stepper with the global StepperRunner
	 */
	bool registerRunner(const String &id) override {
		_id = id;

		// Create shared_ptr without taking ownership of the object
		// (no-op deleter)
		std::shared_ptr<IRunnable> runnable(static_cast<IRunnable *>(this),
											[](IRunnable *) {});
		return StepperRunner::getInstance().registerRunnable(runnable);
	}

	bool unregisterRunner(const String &id) override {
		return StepperRunner::getInstance().unregisterRunnable(id);
	}

	void setEnablePin(uint8_t en) { this->en = en; }
	void setStepPin(uint8_t step) { this->step = step; }
	void setDirPin(uint8_t dir) { this->dir = dir; }
	void setStepsPerRevolution(int steps) { spr = steps; }
	void setMicrosteps(int microsteps) { this->microsteps = microsteps; }

	// void setSleepPin(int sleep) { this->sleep = sleep; }
	uint8_t getEnablePin() { return en; }
	uint8_t getStepPin() { return step; }
	uint8_t getDirPin() { return dir; }

	uint8_t getStepsPerRevolution() { return spr; }

  private:
	String _id;		// ID of the stepper
	int spr;		// steps per revolution
	int rpm;		// revolutions per minute
	int microsteps; // microsteps per step
	uint8_t sleep;	// sleep pin
	uint8_t en;		// enable pin
	uint8_t step;	// step pin
	uint8_t dir;	// direction pin
	AccelStepper stepper;
};

// ===================== Adapter TMC2130 =====================
/*

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
*/
#endif