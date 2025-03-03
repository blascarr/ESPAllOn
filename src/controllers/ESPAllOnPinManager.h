#ifndef _ESPALLON_PIN_MANAGER_H
#define _ESPALLON_PIN_MANAGER_H

#include "../utils.h"

#include "../models/ESP_Boards.h"
#include <PinManager.h>

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

// ---------------------------------------------------------- //
// --------------------ESPAllOnPinManager-------------------- //
// ---------------------------------------------------------- //
/*	ESPAllOnPinManager define a structure where PIN Labels works with GUI
 *	in order to review which Pins can be used in different ESPinner Mods.
 *	ESPAllOn Object will manage ESPAllOnPinManager with a pointer reference.
 *	But maybe is not necessary, because ESPAllOnPinManager has an instance
 * 	in order to check it as a SingleTon Pattern.
 */
class ESPAllOnPinManager : public PinManager<ESP_BoardConf, ESP_PinMode> {

  public:
	// Relation PIN with Selector Reference in ESPAllOn GUI
	// <uint16_t PIN , uint16_ t SELECTOR>
	std::map<uint16_t, uint16_t> pinCurrentStatus;

	ESPAllOnPinManager() {}

	static ESPAllOnPinManager &getInstance() {
		static ESPAllOnPinManager instance;
		return instance;
	}

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

	void debugCurrentStatus() { debugMap(pinCurrentStatus); }

	void setPinControlRelation(uint8_t pin, uint16_t espuiControlRef) {
		pinCurrentStatus[pin] = espuiControlRef;
	}
};

const char *getPinTypeName(PinType pinType) {
	auto it = pinTypeNames.find(pinType);
	if (it != pinTypeNames.end()) {
		return it->second;
	}
	return "Unknown";
}

// DUMP PINOUT LIST
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

bool isNumericAndInRange(const String value) {

	// Avoid selected pin 0
	int val = value.toInt();
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

	return false;
}

#endif