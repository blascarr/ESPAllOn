#ifndef _ESPALLON_BOARDS_H
#define _ESPALLON_BOARDS_H

#include "PinManager.h"
#if defined(ESP32)
struct ESP_BoardConf {
	static constexpr size_t NUM_PINS = 40;
	static ESP_PinMode PINOUT[NUM_PINS];
};

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
struct ESP_BoardConf {
	static constexpr size_t NUM_PINS = 20;
	static ESP_PinMode PINOUT[NUM_PINS];
#if DEBUG
	static constexpr size_t INITIAL_PINS = 7;
#else
	static constexpr size_t INITIAL_PINS = 9;
#endif
};

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