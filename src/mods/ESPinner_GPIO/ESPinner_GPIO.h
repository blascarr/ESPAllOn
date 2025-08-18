#ifndef _ESPINNER_GPIO_H
#define _ESPINNER_GPIO_H

#include "../../controllers/ESPAllOnPinManager.h"
#include <ESPUI.h>

/**
 * Structure defining GPIO mode options
 */
struct ESPinner_GPIOMode {
	GPIOMode model; ///< GPIO mode enumeration
	String name;	///< Human-readable name for the mode
};

/** Available GPIO modes with their labels */
const ESPinner_GPIOMode GPIO_mods[] = {
	{GPIOMode::Input, GPIO_ESPINNERINPUT_LABEL},
	{GPIOMode::Output, GPIO_ESPINNEROUTPUT_LABEL}};

/**
 * Creates a GPIO mode selector UI component
 * @param GPIO_ptr Parent UI element reference
 * @return Reference to the created selector
 */
uint16_t GPIO_ModeSelector(uint16_t GPIO_ptr);

/**
 * GPIO implementation of ESPinner for basic digital I/O operations
 *
 * ESPinner_GPIO provides functionality for configuring and managing GPIO pins
 * as either inputs or outputs.
 */

class ESPinner_GPIO : public ESPinner {
  public:
	uint8_t gpio;		// GPIO pin number
	GPIOMode GPIO_mode; // Current GPIO mode (Input/Output)

	/**
	 * Constructor with specific GPIO mode
	 * @param espinner_mod GPIO mode to initialize with
	 */
	ESPinner_GPIO(GPIOMode espinner_mod)
		: ESPinner(ESPinner_Mod::GPIO), GPIO_mode(espinner_mod) {}

	/**
	 * Default constructor - initializes with undefined mode
	 */
	ESPinner_GPIO()
		: ESPinner(ESPinner_Mod::GPIO), GPIO_mode(GPIOMode::Undefined) {}

	void setup() override { DUMPSLN("Iniciacion configuración de GPIO..."); }

	void update() override { DUMPSLN("Update configuración de GPIO..."); }

	void loader() override { DUMPSLN("Cargando configuración de GPIO..."); }

	/** Implement GPIO functionality and integrate with pin manager */
	void implement() override;

	/**
	 * Sets the GPIO pin number
	 * @param gpio_pin Pin number to assign
	 */
	void setGPIO(uint8_t gpio_pin) { gpio = gpio_pin; }

	/**
	 * Gets the current GPIO pin number
	 * @return Current GPIO pin number
	 */
	uint8_t getGPIO() { return gpio; }

	/**
	 * Sets the GPIO mode using enumeration
	 * @param mode GPIO mode enumeration value
	 */
	void setGPIOMode(GPIOMode mode) { GPIO_mode = mode; }

	/**
	 * Sets the GPIO mode using string configuration
	 * @param mode String representation of GPIO mode
	 */
	void setGPIOMode(String mode) {
		if (mode == ESPINNER_INPUT_CONFIG) {
			setGPIOMode(GPIOMode::Input);
		}
		if (mode == ESPINNER_OUTPUT_CONFIG) {
			setGPIOMode(GPIOMode::Output);
		}
	}

	/**
	 * Gets the current GPIO mode
	 * @return Current GPIO mode enumeration
	 */
	GPIOMode getGPIOMode() { return GPIO_mode; }

	/**
	 * Gets the GPIO mode as JSON configuration string
	 * @return String representation for JSON serialization
	 */
	String getGPIOMode_JSON() {
		if (GPIO_mode == GPIOMode::Input) {
			return ESPINNER_INPUT_CONFIG;
		}
		if (GPIO_mode == GPIOMode::Output) {
			return ESPINNER_OUTPUT_CONFIG;
		}
		return "";
	}

	/**
	 * Gets the pin mode configuration for the pin manager
	 * @return ESP_PinMode configuration structure
	 *
	 * Creates the appropriate pin configuration based on the current GPIO mode.
	 * Input pins are configured as BusDigital, output pins as BusPWM.
	 * Defaults to output mode if undefined.
	 */
	ESP_PinMode getPinModeConf() {

		switch (GPIO_mode) {
		case GPIOMode::Input: {
			InputPin model = InputPin(false, false);
			ESP_PinMode pinMode = {this->getGPIO(), model, PinType::BusDigital};
			return pinMode;
		}
		case GPIOMode::Output: {
			OutputPin model = OutputPin(true);
			ESP_PinMode pinMode = {this->getGPIO(), model, PinType::BusPWM};
			return pinMode;
		}
		// If no GPIO_Mode is configured, return as Output by default
		default:
			ESP_PinMode pinMode = {this->getGPIO(), OutputPin(true),
								   PinType::BusPWM};
			return pinMode;
		}
	};

	/**
	 * Serializes GPIO configuration to JSON
	 * @return JsonDocument containing the serialized configuration
	 *
	 * Creates a JSON representation of the GPIO configuration including
	 * model type, ID, pin number, and I/O mode for persistent storage.
	 */
	JsonDocument serializeJSON() override {
		StaticJsonDocument<256> doc;
		doc[ESPINNER_MODEL_JSONCONFIG] = ESPINNER_GPIO_JSONCONFIG;
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

	/**
	 * Deserializes GPIO configuration from JSON string
	 * @param data JSON string containing the configuration
	 * @return True if deserialization was successful, false otherwise
	 */
	bool deserializeJSON(const String &data) {
		StaticJsonDocument<256> doc;
		DeserializationError error = deserializeJson(doc, data);
		if (error) {
			return false;
		}
		ESPinner_GPIO::setGPIO(doc[ESPINNER_GPIO_JSONCONFIG].as<int>());
		String ID = doc[ESPINNER_ID_JSONCONFIG].as<const char *>();
		ESPinner::setID(ID);
		String GPIOMODE = doc[ESPINNER_IO_JSONCONFIG].as<const char *>();
		setGPIOMode(GPIOMODE);

		return true;
	};
};

// Forward declarations for callback functions and UI creation

/**
 * Callback for creating new GPIO modules
 */
void createGPIOMod_callback(Control *sender, int type);

/**
 * Callback for saving GPIO configuration
 */
void saveGPIO_callback(Control *sender, int type);

/**
 * Callback for removing GPIO configuration
 */
void removeGPIO_callback(Control *sender, int type);

/**
 * Callback for GPIO pin selector changes
 */
void GPIOSelector_callback(Control *sender, int type);

/**
 * Creates GPIO pin selector UI component
 * @param PIN_ptr Parent UI element reference
 */
void GPIO_Selector(uint16_t PIN_ptr);

/**
 * Creates complete GPIO configuration UI
 * @param GPIO_ptr Parent UI element reference
 */
void GPIO_UI(uint16_t GPIO_ptr);

/**
 * Callback for GPIO switch/toggle controls
 */
void GPIOSwitcher_callback(Control *sender, int type);

#endif