#ifndef _ESPALLON_PIN_MANAGER_H
#define _ESPALLON_PIN_MANAGER_H
#include "ESP_Boards.h"
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
class ESPAllOnPinManager : public PinManager<ESP_BoardConf, PinMode> {

  public:
	ESPAllOnPinManager() {}
	static ESPAllOnPinManager &getInstance() {
		static ESPAllOnPinManager instance;
		return instance;
	}
};

#define VERBOSE false

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