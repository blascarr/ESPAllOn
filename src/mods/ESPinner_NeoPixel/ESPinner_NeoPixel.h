#ifndef _ESPINNER_NEOPIXEL_H
#define _ESPINNER_NEOPIXEL_H

#include "../../controllers/ESPAllOnPinManager.h"
#include "NeopixelRunner.h"

#include <ESPUI.h>
#include <TickerFree.h>

class ESPinner_Neopixel : public ESPinner, public INeopixelRunnable {
  private:
	bool _enabled = true;
	bool _rainbowMode = false;
	float _speed = 1.0f;
	uint32_t _currentColor = 0xFFFFFF;
	uint32_t _rainbowHue = 0;
	unsigned long _lastUpdate = 0;

	// Animation state variables
	NEOPIXEL_ANIMATION _currentAnimation = NEOPIXEL_ANIMATION::SOLID;
	uint8_t _currentLED = 0;
	uint8_t _currentCounter = 0;
	uint8_t _targetCounter = 0;
	bool _reverseDirection = false;
	bool _loopAnimation = true;
	bool _bounceAnimation = true;
	int _fadeBrightness = DEFAULT_BRIGHTNESS;
	uint8_t _fadeIncrement = 10;
	int _pixelCycle = 0;

	status_t currentState;
	status_t previousState;
	StateChangeCallback onChangeCallback = nullptr;

	Adafruit_NeoPixel strip;

  public:
	ESPinner_Neopixel()
		: ESPinner(ESPinner_Mod::NeoPixel),
		  neopixelTicker([this]() { this->clearLeds(); }, NEOPIXEL_INTERVAL_MS,
						 1, MILLIS) {
		this->setup();
	}

	ESPinner_Neopixel(int gpioPin, int numPixels)
		: ESPinner(ESPinner_Mod::NeoPixel),
		  strip(numPixels, gpioPin, NEO_GRB + NEO_KHZ800),
		  neopixelTicker([this]() { this->clearLeds(); }, NEOPIXEL_INTERVAL_MS,
						 1, MILLIS) {
		this->setup();
	}

	ESP_PinMode getPinModeConf() {
		ESP_PinMode pinMode = {this->getGPIO(), OutputPin(true),
							   PinType::BusDigital};
		return pinMode;
	};

	void setup() override {
		DUMPSLN("Iniciacion configuración de NEOPIXEL...");
		strip.begin();
		strip.clear();
		strip.show();
	}

	void implement() override;
	void update() override {
		// Execute current animation
		switch (_currentAnimation) {
		case NEOPIXEL_ANIMATION::SOLID:
			// Do nothing
			break;

		case NEOPIXEL_ANIMATION::BLINK:
			animateBlink();
			break;
		case NEOPIXEL_ANIMATION::FADE:
			animateFade();
			break;
		case NEOPIXEL_ANIMATION::INCREMENTAL:
			animateIncremental();
			break;
		case NEOPIXEL_ANIMATION::DECREMENTAL:
			animateDecremental();
			break;
		case NEOPIXEL_ANIMATION::RAINBOW:
			animateRainbow();
			break;
		case NEOPIXEL_ANIMATION::CLEAR:
			// Do nothing
			break;
		default:
			break;
		}
	}
	void loader() override { DUMPSLN("Cargando configuración de NEOPIXEL..."); }
	void clearLeds() {
		strip.clear();
		strip.show();
	}
	void run() override {
		if (neopixelTicker.state() == status_t::RUNNING) {
			neopixelTicker.update();
		}
	}
	bool isActive() const override { return INeopixelRunnable::isEnabled(); }

	/**
	 * Enable or disable the NeoPixel strip
	 * @param on True to enable, false to disable
	 */
	void enable(bool on) override {
		_enabled = on;
		INeopixelRunnable::enable(on); // Update _active in base class

		if (on) {
			// Start the ticker if we have an animation
			if (_currentAnimation != NEOPIXEL_ANIMATION::CLEAR) {
				if (neopixelTicker.state() != status_t::RUNNING) {
					neopixelTicker.start();
				}
			}
		} else {
			// Stop ticker and clear LEDs
			neopixelTicker.stop();
			strip.clear();
			strip.show();
		}
	}

	// INeopixelRunnable interface implementation
	void setGPIO(int gpioPin) {
		strip.setPin(gpioPin);
		strip.begin(); // Reinitialize after pin change
	}
	void setNumPixels(int numPixels) {
		strip.updateLength(numPixels);
		strip.begin(); // Reinitialize after length change
	}

	void setRainbowMode(bool enabled) {
		_rainbowMode = enabled;
		if (enabled) {
			setAnimation(NEOPIXEL_ANIMATION::RAINBOW,
						 static_cast<uint32_t>(50 / _speed));
		} else {
			stopAnimation();
		}
	}
	void setSpeed(float speed) {
		_speed = constrain(speed, 0.1f, 10.0f);
		if (_rainbowMode) {
			// Update ticker interval with new speed
			neopixelTicker.interval(static_cast<uint32_t>(50 / _speed));
		}
	}
	void setBrightness(uint8_t brightness) {
		strip.setBrightness(brightness);
		strip.show();
	}
	void setColor(uint32_t color) {
		_currentColor = color;

		// Debug: Show color conversion
		RGBColor rgb = hexToRGB(color);
		DUMPLN("Setting color - Hex: 0x", String(color, HEX));
		DUMPLN("RGB - R:", rgb.val[0]);
		DUMPLN("RGB - G:", rgb.val[1]);
		DUMPLN("RGB - B:", rgb.val[2]);

		// Apply color immediately if in SOLID mode or not in rainbow mode
		if (!_rainbowMode && _enabled) {
			if (_currentAnimation == NEOPIXEL_ANIMATION::SOLID) {
				fillLeds(); // Use our new fillLeds method
			} else {
				// For other animations, fill with the new color
				strip.fill(color, 0, strip.numPixels());
				strip.show();
			}
		}
	}

	// Configuration method
	void configureStrip(int gpioPin, int numPixels) {
		strip.setPin(gpioPin);
		strip.updateLength(numPixels);
		strip.begin();
		strip.clear();
		strip.show();
	}

	/**
	 * Set animation type and start it
	 * @param animation Type of animation to run
	 * @param interval_ms Interval in milliseconds between animation updates
	 * @param times Number of times to repeat (0 = infinite)
	 */
	void setAnimation(NEOPIXEL_ANIMATION animation, uint32_t interval_ms = 50,
					  uint32_t times = 0) {
		_currentAnimation = animation;
		switch (animation) {
		case SOLID:
			DUMPSLN("Setting solid animation");
			fillLeds(); // Fill immediately with current color
			// No ticker needed for solid color
			break;

		case CLEAR:
			this->setCallback(
				std::bind(&ESPinner_Neopixel::animateRainbow, this),
				interval_ms, times);

			break;

		case BLINK:
			this->setCallback(
				std::bind(&ESPinner_Neopixel::animateRainbow, this),
				interval_ms, times);

			break;

		case FADE:
			this->setCallback(
				std::bind(&ESPinner_Neopixel::animateRainbow, this),
				interval_ms, times);
			break;

		case INCREMENTAL:
			this->setCallback(
				std::bind(&ESPinner_Neopixel::animateRainbow, this),
				interval_ms, times);
			break;

		case DECREMENTAL:
			this->setCallback(
				std::bind(&ESPinner_Neopixel::animateDecremental, this),
				interval_ms, times);

			break;

		case RAINBOW:
			this->setCallback(
				std::bind(&ESPinner_Neopixel::animateRainbow, this),
				interval_ms, times);
			break;

		default:

			break;
		}
		resetAnimation();

		// Configure ticker with new interval
		neopixelTicker.interval(interval_ms);

		// Start ticker if not running
		if (neopixelTicker.state() != status_t::RUNNING) {
			neopixelTicker.start();
		}
	}

	/**
	 * Stop current animation
	 */
	void stopAnimation() {
		_currentAnimation = NEOPIXEL_ANIMATION::CLEAR;
		neopixelTicker.stop();
		strip.clear();
		strip.show();
	}

	/**
	 * Reset animation state variables
	 */
	void resetAnimation() {
		_currentLED = 0;
		_currentCounter = 0;
		_reverseDirection = false;
		_pixelCycle = 0;
		_fadeBrightness = strip.getBrightness();
	}

	// Getters and setters
	int getGPIO() const { return strip.getPin(); }
	int getNumPixels() const { return strip.numPixels(); }
	bool getRainbowMode() const { return _rainbowMode; }
	float getSpeed() const { return _speed; }
	uint32_t getCurrentColor() const { return _currentColor; }
	NEOPIXEL_ANIMATION getCurrentAnimation() const { return _currentAnimation; }

	// Animation configuration getters/setters
	void setLoopAnimation(bool loop) { _loopAnimation = loop; }
	void setBounceAnimation(bool bounce) { _bounceAnimation = bounce; }
	void setFadeIncrement(uint8_t increment) { _fadeIncrement = increment; }
	bool getLoopAnimation() const { return _loopAnimation; }
	bool getBounceAnimation() const { return _bounceAnimation; }
	uint8_t getFadeIncrement() const { return _fadeIncrement; }

	/**
	 * Register this NeoPixel with the NeopixelRunner
	 * This should be called after the NeoPixel is fully configured
	 */

	bool registerRunner(const String &id) {
		// Set the ID for the runnable interface
		// this->ID = id;

		// Create shared_ptr without taking ownership of the object
		// (no-op deleter since it's managed by ESPinner_Manager)
		auto neopixelPtr = std::shared_ptr<INeopixelRunnable>(
			this, [](INeopixelRunnable *) {});

		return NeopixelRunner::getInstance().registerRunnable(neopixelPtr);
	}

	JsonDocument serializeJSON() override {
		JsonDocument doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = ESPINNER_NEOPIXEL_JSONCONFIG;
		doc[ESPINNER_ID_JSONCONFIG] = ESPinner::getID();
		doc[ESPINNER_NEOPIXEL_GPIO_CONFIG] = strip.getPin();
		doc[ESPINNER_NEOPIXEL_NUMPIXELS_CONFIG] = strip.numPixels();
		return doc;
	}

	bool deserializeJSON(const String &data) {
		JsonDocument doc;
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

	// =============== ANIMATION METHODS ===============

	/**
	 * Convert uint32_t hex color to RGBColor struct
	 * @param hexColor Color in 0xRRGGBB format
	 * @return RGBColor struct with RGB values
	 */
	RGBColor hexToRGB(uint32_t hexColor) {
		RGBColor rgb;
		rgb.val[0] = (hexColor >> 16) & 0xFF; // Red
		rgb.val[1] = (hexColor >> 8) & 0xFF;  // Green
		rgb.val[2] = hexColor & 0xFF;		  // Blue
		return rgb;
	}

	/**
	 * Get current NeoPixel color as RGBColor
	 * @return RGBColor struct with current color values
	 */
	RGBColor getNeoPixelColor() { return hexToRGB(_currentColor); }

	/**
	 * Fill all LEDs with current color
	 */
	void LEDSON() {
		if (_enabled) {
			RGBColor colorCode = getNeoPixelColor();
			// Use Adafruit_NeoPixel's Color function to create proper color
			// value
			uint32_t color = strip.Color(colorCode.val[0], colorCode.val[1],
										 colorCode.val[2]);
			strip.fill(color, 0, strip.numPixels());
			strip.show();
		} else {
			clearLeds();
		}
	}

	/**
	 * Fill all LEDs with solid color (for SOLID animation)
	 */
	void fillLeds() {
		if (_enabled) {
			strip.fill(_currentColor, 0, strip.numPixels());
			strip.show();
		}
	}

	/**
	 * Blink animation - toggles all LEDs on/off
	 */
	void animateBlink() {
		static bool ledState = false;

		if (ledState) {
			// Turn on with current color
			strip.fill(_currentColor, 0, strip.numPixels());
		} else {
			// Turn off
			strip.clear();
		}
		strip.show();
		ledState = !ledState;
	}

	/**
	 * Fade animation - fades in and out
	 */
	void animateFade() {
		static bool fadingIn = true;

		if (fadingIn) {
			_fadeBrightness += _fadeIncrement;
			if (_fadeBrightness >= 255) {
				_fadeBrightness = 255;
				fadingIn = false;
			}
		} else {
			_fadeBrightness -= _fadeIncrement;
			if (_fadeBrightness <= 0) {
				_fadeBrightness = 0;
				fadingIn = true;
			}
		}

		strip.setBrightness(_fadeBrightness);
		strip.fill(_currentColor, 0, strip.numPixels());
		strip.show();
	}

	/**
	 * Incremental animation - lights up LEDs one by one
	 */
	void animateIncremental() {
		// Clear previous LED if reversing
		if (_reverseDirection) {
			strip.setPixelColor(_currentLED, 0);
			_currentLED--;

			if (_currentLED < 0) {
				_currentCounter++;
				if (_bounceAnimation) {
					_reverseDirection = false;
					_currentLED = 0;
				} else if (_loopAnimation) {
					_currentLED = strip.numPixels() - 1;
				}
			}
		} else {
			// Light up current LED
			strip.setPixelColor(_currentLED, _currentColor);
			_currentLED++;

			if (_currentLED >= strip.numPixels()) {
				_currentCounter++;
				if (_bounceAnimation) {
					_reverseDirection = true;
					_currentLED = strip.numPixels() - 1;
				} else if (_loopAnimation) {
					strip.clear();
					_currentLED = 0;
				}
			}
		}

		strip.show();
	}

	/**
	 * Decremental animation - turns off LEDs one by one
	 */
	void animateDecremental() {
		// Start with all LEDs on
		if (_currentLED == 0 && _currentCounter == 0) {
			strip.fill(_currentColor, 0, strip.numPixels());
			strip.show();
		}

		// Turn off current LED
		strip.setPixelColor(_currentLED, 0);
		_currentLED++;

		if (_currentLED >= strip.numPixels()) {
			_currentCounter++;
			if (_loopAnimation) {
				// Reset and fill again
				strip.fill(_currentColor, 0, strip.numPixels());
				_currentLED = 0;
			}
		}

		strip.show();
	}

	/**
	 * Rainbow animation - cycles through rainbow colors
	 */
	void animateRainbow() {
		for (uint16_t i = 0; i < strip.numPixels(); i++) {
			// Calculate color using HSV
			uint32_t color = strip.ColorHSV(
				(_pixelCycle + (i * 65536L / strip.numPixels())) % 65536);
			strip.setPixelColor(i, color);
		}
		strip.show();

		// Advance cycle
		_pixelCycle += static_cast<int>(256 * _speed);
		if (_pixelCycle >= 65536) {
			_pixelCycle = 0;
		}
	}

	/**
	 * Utility function to create colors from RGB values
	 */
	static uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
		return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
	}

	/**
	 * Utility function for rainbow wheel colors
	 */
	static uint32_t Wheel(byte wheelPos) {
		wheelPos = 255 - wheelPos;
		if (wheelPos < 85) {
			return Color(255 - wheelPos * 3, 0, wheelPos * 3);
		}
		if (wheelPos < 170) {
			wheelPos -= 85;
			return Color(0, wheelPos * 3, 255 - wheelPos * 3);
		}
		wheelPos -= 170;
		return Color(wheelPos * 3, 255 - wheelPos * 3, 0);
	}

	void setCallback(CallbackType cb, uint32_t timer = 1000,
					 uint32_t repeat = 0, resolution_t resolution = MICROS) {
		CallbackType combined = combinedCallback(cb);
		neopixelTicker.setCallback(combined);
		neopixelTicker.interval(timer);
		// neopixelTicker.repeats(repeat);
	}
	// CallbackType getCallback() const { return neopixelTicker.getCallback(); }

	void checkAndUpdateState() {
		currentState = neopixelTicker.state();
		if (currentState != previousState) {
			if (onChangeCallback) {
				// onChangeCallback(this->getControllerType(), previousState,
				// currentState);
			}
			previousState = currentState;
		}
	}

	void setOnChangeCallback(StateChangeCallback callback) {
		// onChangeCallback = callback;
	}

	CallbackType combinedCallback(CallbackType cb) {
		return [this, cb]() {
			if (cb) {
				cb();
			}
			checkAndUpdateState();
		};
	}
};

#endif