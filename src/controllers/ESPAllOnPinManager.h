#ifndef _ESPALLON_PIN_MANAGER_H
#define _ESPALLON_PIN_MANAGER_H

#include "../utils.h"

#include "../models/ESP_Boards.h"
#include <PinManager.h>

/** Mapping of pin types to their string representations */
const std::map<PinType, const char *> pinTypeNames = {
	{PinType::None, "None"},
	{PinType::Ethernet, "Ethernet"},
	{PinType::BusDigital, "BusDigital"},
	{PinType::BusOnOff, "BusOnOff"},
	{PinType::BusPWM, "BusPWM"},
	{PinType::Button, "Button"},
	{PinType::IR, "IR"},
	{PinType::Relay, "Relay"},
	{PinType::SPI_RAM, "SPI_RAM"},
	{PinType::DebugOut, "DebugOut"},
	{PinType::DMX, "DMX"},
	{PinType::HW_I2C_SDA, "HW_I2C_SDA"},
	{PinType::HW_I2C_SCL, "HW_I2C_SCL"},
	{PinType::HSPI_MISO, "HSPI_MISO"},
	{PinType::HSPI_MOSI, "HSPI_MOSI"},
	{PinType::HSPI_CLK, "HSPI_CLK"},
	{PinType::HSPI_CS, "HSPI_CS"},
	{PinType::VSPI_MISO, "VSPI_MISO"},
	{PinType::VSPI_MOSI, "VSPI_MOSI"},
	{PinType::VSPI_CLK, "VSPI_CLK"},
	{PinType::VSPI_CS, "VSPI_CS"}};

/**
 * ESPAllOnPinManager extends the base PinManager to provide ESP-specific
 * functionality and integration with the ESPUI library. It manages pin
 * assignments, tracks pin usage, and provides validation for pin operations.
 *
 * The class implements the Singleton pattern to ensure only one instance
 * manages the pins throughout the application lifecycle.
 */

class ESPAllOnPinManager : public PinManager<ESP_BoardConf, ESP_PinMode> {

  public:
	/** Map relating PIN with Selector Reference in ESPAllOn GUI < uint16_t PIN,
	 * uint16_ t SELECTOR_ID>
	 */
	std::map<uint16_t, uint16_t> pinCurrentStatus;

	/**
	 * Marks all broken pins as occupied with a special value (10000)
	 * to prevent them from being used in the system.
	 */
	ESPAllOnPinManager() {
		for (const auto &config : ESP_BoardConf::PINOUT) {
			if (config.isBroken) {
				// Broken Pins should be occupied in list
				pinCurrentStatus[config.pin] = 10000;
			}
		}
	}

	/**
	 * Gets the singleton instance of ESPAllOnPinManager
	 * @return Reference to the singleton instance
	 */
	static ESPAllOnPinManager &getInstance() {
		static ESPAllOnPinManager instance;
		return instance;
	}

	/**
	 * Gets reference to the PIN mapping
	 * @return Reference to the map containing pin-to-selector relationships
	 */
	std::map<uint16_t, uint16_t> &getPINMap() { return pinCurrentStatus; }

	/**
	 * Gets the current pin number for a given reference ID
	 * @param ref Reference ID to search for
	 * @return Pin number if found, 0 otherwise
	 */
	uint16_t getCurrentReference(uint8_t ref) {
		for (auto &relation : pinCurrentStatus) {
			if (relation.second == ref) {
				uint8_t currentPin = relation.first;
				return currentPin;
				break;
			}
		}
		return 0;
	}

	/**
	 * Updates GPIO configuration from ESPUI interface
	 * @param ESPin ESP board Pin configuration model
	 * @param ref UI control reference
	 */
	void updateGPIOFromESPUI(ESP_PinMode ESPin, uint16_t ref) {
		uint16_t sel = getCurrentReference(ref);
		if (sel == 0) {
			setPinControlRelation(ESPin.pin, ref);
			ESPAllOnPinManager::attach(ESPin.pin);
		} else {
			ESPAllOnPinManager::detach(sel);
			ESPAllOnPinManager::attach(ESPin);
			// Remove ref relation with older Pin
			pinCurrentStatus.erase(sel);
			setPinControlRelation(ESPin.pin, ref);
		}
	}

	/**
	 * Debug function to print current pin status
	 */
	void debugCurrentStatus() { debugMap(pinCurrentStatus); }

	/**
	 * Sets the relationship between a pin and its UI control reference
	 * @param pin Pin number
	 * @param espuiControlRef UI control reference ID
	 */
	void setPinControlRelation(uint8_t pin, uint16_t espuiControlRef) {
		pinCurrentStatus[pin] = espuiControlRef;
	}
};

/**
 * Gets the string name for a given pin type
 */
const char *getPinTypeName(PinType pinType) {
	auto it = pinTypeNames.find(pinType);
	if (it != pinTypeNames.end()) {
		return it->second;
	}
	return "Unknown";
}

/**
 * Debug function to dump complete pinout information
 */
void DUMP_PINOUT() {
	for (int i = 0; i < ESP_BoardConf::NUM_PINS; i++) {
		PinType GPIOType = ESPAllOnPinManager::getInstance().getPinType(i);
		DUMP("PIN ", i);
		DUMP(" GPIO ", ESPAllOnPinManager::getInstance().getPin(i));
		DUMPLN(" ATTACHED ",
			   ESPAllOnPinManager::getInstance().isPinAttached(i));
		DUMPLN(" IS PIN OK ", ESPAllOnPinManager::getInstance().isPinOK(i));
		DUMP(" Type ", getPinTypeName(GPIOType));
#if (VERBOSE)

		DUMPLN(" IS I2C ", isI2C(GPIOType));
		DUMPLN(" IS SPI ", isSPI(GPIOType));
		ESP_PinMode GPIO = ESPAllOnPinManager::getInstance().getGPIO(i);
		DUMPLN(" MODE Undefined ", GPIO.mode == GPIOMode::Undefined);
		DUMPLN(" Broken ", GPIO.isBroken);
		DUMPLN(" canDeepSleep ", GPIO.canDeepSleep);
		DUMPLN(" UseWifi ", GPIO.canUseWithWiFi);
		DUMPLN(" TouchGPIO ", GPIO.isTouchGPIO);
#endif
	}
}

/**
 * Validates if a string value represents a valid and available pin
 *
 * This function performs comprehensive validation including:
 * - Numeric validation
 * - Range checking
 * - Broken pin detection
 * - Availability checking
 * - Current assignment validation
 */
bool isNumericAndInRange(const String value, uint16_t ref) {
	int val = value.toInt();
	ESPAllOnPinManager::getInstance().debugCurrentStatus();
	uint16_t pinRef = ESPAllOnPinManager::getInstance().pinCurrentStatus[val];
	if (val == 0 || pinRef == 10000) {
		return false;
	}

	if (ESPAllOnPinManager::getInstance().isPinOK(val)) {
		int lastValue =
			ESPAllOnPinManager::getInstance().getCurrentReference(ref);

		// Avoid selected pin 0
		if (val == lastValue) {
			return true;
		}
		if (val != 0) {
			String numString = String(val);
			if (numString.length() != value.length()) {
				return false;
			}
			// Review if Pin is already occupied in ESPinner_PinManager
			bool is_gpio_attached =
				ESPAllOnPinManager::getInstance().isPinAttached(val);
			if (!is_gpio_attached && val > 0 && val < PINSIZE) {
				return true;
			}
		}
	} else {
		DUMPLN("PIN IS BROKEN ", val);
	}

	return false;
}

#endif