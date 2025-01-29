#ifndef _ESPALLON_BOARDS_H
#define _ESPALLON_BOARDS_H

#include "PinManager.h"
#if defined(ESP32)
struct ESP_BoardConf {
	static constexpr size_t NUM_PINS = 40;
	static ESP_PinMode PINOUT[NUM_PINS];
};
#endif

#if defined(ESP8266)
struct ESP_BoardConf {
	static constexpr size_t NUM_PINS = 20;
	static ESP_PinMode PINOUT[NUM_PINS];
};
#endif

ESP_PinMode ESP_BoardConf::PINOUT[ESP_BoardConf::NUM_PINS] = {

};
#endif