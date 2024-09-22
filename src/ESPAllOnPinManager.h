#ifndef _ESPALLON_PIN_MANAGER_H
#define _ESPALLON_PIN_MANAGER_H
#include "ESP_Boards.h"

#include <Persistance.h>
#include <PinManager.h>
#include <PinSerializable.h>

enum class labelPin {
	PIN0 = 0,
	PIN1 = 1,
	PIN2 = 2,
	PIN3 = 3,
	PIN4 = 4,
	PIN5 = 5,
	PIN6 = 6,
	PIN7 = 7,
	PIN8 = 8,
	PIN9 = 9,
	PIN10 = 10,
	PIN11 = 11,
	PIN12 = 12,
	PIN13 = 13,
	PIN14 = 14,
	PIN15 = 15,
	PIN16 = 16,
	PIN17 = 17,
	PIN18 = 18,
	PIN19 = 19,
	PIN20 = 20,
	PIN21 = 21,
	PIN22 = 22,
	PIN23 = 23,
	PIN24 = 24,
	PIN25 = 25,
	PIN26 = 26,
	PIN27 = 27,
	PIN28 = 28,
	PIN29 = 29,
	PIN30 = 30,
	PIN31 = 31,
	PIN32 = 32,
	PIN33 = 33,
	PIN34 = 34,
	PIN35 = 35,
	PIN36 = 36,
	PIN37 = 37,
	PIN38 = 38,
	PIN39 = 39,
	PIN40 = 40,
};

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
class ESPAllOnPinManager : public PinManager<ESP_BoardConf, PinMode> {

  public:
	std::map<uint8_t, std::string> gpioLabels;
	std::vector<std::string> pinLabels;
	ESPAllOnPinManager() {
		pinLabels.reserve(ESP_BoardConf::NUM_PINS);
		for (int i = 0; i <= ESP_BoardConf::NUM_PINS; i++) {
			pinLabels.push_back(std::to_string(i));
			gpioLabels[i] = std::to_string(i);
		}
	}

	static ESPAllOnPinManager &getInstance() {
		static ESPAllOnPinManager instance;
		return instance;
	}

	void printLabels() {
		for (const std::string &label : pinLabels) {
			DUMPLN("PIN ", label.c_str());
		}
	}
};

const char *getLabelFromPinManager(uint8_t pin) {
	if (ESPAllOnPinManager::getInstance().gpioLabels.find(pin) !=
		ESPAllOnPinManager::getInstance().gpioLabels.end()) {
		return ESPAllOnPinManager::getInstance().gpioLabels[pin].c_str();
		return "";
	} else {
		return "Pin not found";
	}
}
void addGPIOLabelInPinManager(std::pair<uint8_t, const char *> gpioLabelPair) {
	ESPAllOnPinManager::getInstance().gpioLabels[gpioLabelPair.first] =
		gpioLabelPair.second;
}
void removeGPIOLabelFromPinManager(
	std::pair<uint8_t, const char *> gpioLabelPair) {
	ESPAllOnPinManager::getInstance().gpioLabels.erase(gpioLabelPair.first);
}

void removeGPIOLabelFromPinManager(uint8_t gpio) {
	auto it = ESPAllOnPinManager::getInstance().gpioLabels.find(gpio);
	if (it != ESPAllOnPinManager::getInstance().gpioLabels.end()) {
		ESPAllOnPinManager::getInstance().gpioLabels.erase(it);
	}
}

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

#endif