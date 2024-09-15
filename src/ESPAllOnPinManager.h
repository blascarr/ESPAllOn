#ifndef _ESPALLON_PIN_MANAGER_H
#define _ESPALLON_PIN_MANAGER_H
#include "ESP_Boards.h"

#include <Persistance.h>
#include <PinManager.h>
#include <PinSerializable.h>

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

enum class labelPin {
	PIN0 = 0,
	PIN1,
	PIN2,
	PIN3,
	PIN4,
	PIN5,
	PIN6,
	PIN7,
	PIN8,
	PIN9,
	PIN10,
	PIN11,
	PIN12,
	PIN13,
	PIN14,
	PIN15,
	PIN16,
	PIN17,
	PIN18,
	PIN19,
	PIN20,
	PIN21,
	PIN22,
	PIN23,
	PIN24,
	PIN25,
	PIN26,
	PIN27,
	PIN28,
	PIN29,
	PIN30,
	PIN31,
	PIN32,
	PIN33,
	PIN34,
	PIN35,
	PIN36,
	PIN37,
	PIN38,
	PIN39,
	PIN40,
};

// Array of pinLabels to associate with gpioLabels

const std::array<std::string, static_cast<size_t>(PINSIZE)> pinLabels = {
	"1",  "2",	"3",  "4",	"5",  "6",	"7",  "8",	"9",  "10",
	"11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
	"21", "22", "23", "24", "25", "26", "27", "28", "29", "30",
	"31", "32", "33", "34", "35", "36", "37", "38", "39", "40"};

std::string getPinLabel(labelPin pin) {
	return pinLabels[static_cast<size_t>(pin)];
}

// Available List of GPIOs currently

std::map<uint8_t, const char *> gpioLabels = {
	{0, "0"},	{1, "1"},	{2, "2"},	{3, "3"},	{4, "4"},	{5, "5"},
	{6, "6"},	{7, "7"},	{8, "8"},	{9, "9"},	{10, "10"}, {11, "11"},
	{12, "12"}, {13, "13"}, {14, "14"}, {15, "15"}, {16, "16"}, {17, "17"},
	{18, "18"}, {19, "19"}, {20, "20"}, {21, "21"}, {22, "22"}, {23, "23"},
	{24, "24"}, {25, "25"}, {26, "26"}, {27, "27"}, {28, "28"}, {29, "29"},
	{30, "30"}, {31, "31"}, {32, "32"}, {33, "33"}, {34, "34"}, {35, "35"},
	{36, "36"}, {37, "37"}, {38, "38"}, {39, "39"}};

const char *getLabelForPin(uint8_t pin) {
	if (gpioLabels.find(pin) != gpioLabels.end()) {
		return gpioLabels[pin];
	} else {
		return "Pin not found";
	}
}

void addGPIOLabel(std::pair<uint8_t, const char *> gpioLabelPair) {
	addLabelOnList(gpioLabels, gpioLabelPair);
}

void removeGPIOLabel(std::pair<uint8_t, const char *> gpioLabelPair) {
	removeLabelOnList(gpioLabels, gpioLabelPair);
}

void removeGPIOLabel(uint8_t gpio) { removeLabelOnList(gpioLabels, gpio); }

#endif