#ifndef _ESPINNER_NEOPIXEL_H
#define _ESPINNER_NEOPIXEL_H

#include "../../controllers/ESPAllOnPinManager.h"
#include "NeopixelRunner.h"

#include <ESPUI.h>
#include <TickerFree.h>

class ESPinner_Neopixel : public ESPinner, public INeopixelRunnable {
  private:
	bool _power = false;
	bool _rainbowMode = false;
	float _rainbowSpeed = 1.0f;
	uint32_t _currentColor = 0xFFFFFF;
	uint32_t _rainbowHue = 0;
	unsigned long _lastUpdate = 0;

	Adafruit_NeoPixel strip;

  public:
	ESPinner_Neopixel()
		: ESPinner(ESPinner_Mod::NeoPixel),
		  neopixelTicker([this]() { this->update(); }, NEOPIXEL_INTERVAL_MS, 0,
						 MILLIS) {
		this->setup();
	}

	void setup() override {
		DUMPSLN("Iniciacion configuración de NEOPIXEL...");

		strip.begin();
		strip.clear();
		strip.show();
	}

	void implement() override;
	void update() override { DUMPSLN("Update configuración de Stepper ..."); }

	void run() override {
		if (neopixelTicker.state() == status_t::RUNNING) {
			neopixelTicker.update();
		}
	}

	// INeopixelRunnable interface implementation
	bool isActive() const override { return _power; }

	void loader() override { DUMPSLN("Cargando configuración de NEOPIXEL..."); }

	void setRainbowMode(bool enabled) { _rainbowMode = enabled; }

	void setRainbowSpeed(float speed) {
		_rainbowSpeed = constrain(speed, 0.1f, 10.0f);
	}

	void setColor(uint32_t color) { _currentColor = color; }

	void setPower(bool power) { _power = power; }

	// Configuration method
	void configureStrip(int gpioPin, int numPixels) {
		strip.setPin(gpioPin);
		strip.updateLength(numPixels);
		strip.begin();
		strip.clear();
		strip.show();
	}

	// Getters and setters
	int getGPIO() const { return strip.getPin(); }
	int getNumPixels() const { return strip.numPixels(); }

	bool getPower() const { return _power; }

	bool getRainbowMode() const { return _rainbowMode; }

	float getRainbowSpeed() const { return _rainbowSpeed; }

	uint32_t getCurrentColor() const { return _currentColor; }

	/**
	 * Register this NeoPixel with the NeopixelRunner
	 * This should be called after the NeoPixel is fully configured
	 */

	void registerRunner(const String &id) {
		// Create a shared_ptr that doesn't delete the object
		// since it's managed by ESPinner_Manager

		// Create shared_ptr without taking ownership of the object
		// (no-op deleter)

		/*std::shared_ptr<IRunnable> runnable(static_cast<IRunnable *>(this),
											[](IRunnable *) {});
		return StepperRunner::getInstance().registerRunnable(runnable);

		// ------------------------------------------------------------
		bool registered = stepper->registerRunner(this->getID());
		auto neopixelPtr = std::shared_ptr<INeopixelRunnable>(
			this, [](INeopixelRunnable *) {});
		NeopixelRunner::getInstance().registerRunnable(neopixelPtr);
		*/
	}

	JsonDocument serializeJSON() override {
		StaticJsonDocument<512> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = ESPINNER_NEOPIXEL_JSONCONFIG;
		doc[ESPINNER_ID_JSONCONFIG] = ESPinner::getID();
		doc[ESPINNER_NEOPIXEL_GPIO_CONFIG] = strip.getPin();
		doc[ESPINNER_NEOPIXEL_NUMPIXELS_CONFIG] = strip.numPixels();
		return doc;
	}

	bool deserializeJSON(const String &data) {
		StaticJsonDocument<256> doc;
		DeserializationError error = deserializeJson(doc, data);
		if (error) {
			return false;
		}
		String ID = doc[ESPINNER_ID_JSONCONFIG].as<const char *>();
		ESPinner_Neopixel::setID(ID);
		strip.setPin(doc[ESPINNER_NEOPIXEL_GPIO_CONFIG].as<int>());
		strip.updateLength(doc[ESPINNER_NEOPIXEL_NUMPIXELS_CONFIG].as<int>());

		Serial.print(doc[ESPINNER_ID_JSONCONFIG].as<const char *>());
		Serial.print(doc[ESPINNER_NEOPIXEL_NUMPIXELS_CONFIG].as<int>());

		return true;
	}

  private:
	TickerFree<> neopixelTicker;

	void rainbowEffect() {

		_rainbowHue += 2;
		if (_rainbowHue >= 256)
			_rainbowHue = 0;

		for (int i = 0; i < strip.numPixels(); i++) {
			uint32_t color = strip.ColorHSV(_rainbowHue * 256 +
											(i * 65536L / strip.numPixels()));
			strip.setPixelColor(i, color);
		}
		strip.show();
	}
};

#endif