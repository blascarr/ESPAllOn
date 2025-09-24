#ifndef _ESPALLON_BOARDS_H
#define _ESPALLON_BOARDS_H

#include "PinManager.h"

#if defined(ESP32)
/**
 * ESP32 board configuration structure
 * Defines pin layout and capabilities for ESP32 boards
 */
struct ESP_BoardConf {
	static constexpr size_t NUM_PINS = 40; // Total number of pins
	static ESP_PinMode PINOUT[NUM_PINS];   // Pin configuration array
#if DEBUG
	static constexpr size_t INITIAL_PINS = 7; // Debug mode initial pins
#else
	static constexpr size_t INITIAL_PINS = 9; // Release mode initial pins
#endif
};

/** ESP32 pin configuration array with GPIO capabilities */
ESP_PinMode ESP_BoardConf::PINOUT[ESP_BoardConf::NUM_PINS] = {
	{0, InputPin(false, false), PinType::BusDigital, false, true,
	 true}, // GPIO0 (Boot)
#if DEBUG
	{1, InputPin(false, false), PinType::DebugOut, false, true,
	 false}, // GPIO1 (TX)
	{3, InputPin(false, true), PinType::BusPWM, false, true,
	 false}, // GPIO3 (RX)
#endif
	{6, OutputPin(true), PinType::BusDigital, false, true, true},
	{7, OutputPin(true), PinType::BusDigital, false, true, true},
	{8, OutputPin(true), PinType::BusDigital, false, true, true},
	{9, OutputPin(true), PinType::BusPWM, false, true, true},
	{10, OutputPin(true), PinType::BusPWM, false, true, true},
	{11, OutputPin(true), PinType::BusDigital, false, true, true}};
#endif

#if defined(ESP8266)
/**
 * ESP8266 board configuration structure
 * Defines pin layout and capabilities for ESP8266 boards
 */
struct ESP_BoardConf {
	static constexpr size_t NUM_PINS = 17; // Total number of pins
	static ESP_PinMode PINOUT[NUM_PINS];   // Pin configuration array
#if DEBUG
	static constexpr size_t INITIAL_PINS = 7; // Debug mode initial pins
#else
	static constexpr size_t INITIAL_PINS = 9; // Release mode initial pins
#endif
};

/** ESP8266 pin configuration array with GPIO capabilities */
ESP_PinMode ESP_BoardConf::PINOUT[ESP_BoardConf::NUM_PINS] = {
	{0, InputPin(false, false), PinType::BusDigital, false, true,
	 true}, // GPIO0 (Boot)
#if DEBUG
	{1, InputPin(false, false), PinType::DebugOut, false, true,
	 false}, // GPIO1 (TX)
	{3, InputPin(false, true), PinType::DebugOut, false, true,
	 false}, // GPIO3 (RX)
#endif
	{6, OutputPin(true), PinType::BusDigital, false, true, true},
	{7, OutputPin(true), PinType::BusDigital, false, true, true},
	{8, OutputPin(true), PinType::BusDigital, false, true, true},
	{9, OutputPin(true), PinType::BusPWM, false, true, true},
	{10, OutputPin(true), PinType::BusPWM, false, true, true},
	{11, OutputPin(true), PinType::BusDigital, false, true, true}};

#endif

#endif