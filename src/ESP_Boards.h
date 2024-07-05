#include "PinManager.h"
#if defined(ESP32)
struct ESP_BoardConf {
	static constexpr size_t NUM_PINS = 40;
	static const ESP_PinMode PINOUT[NUM_PINS];
};
#endif

#if defined(ESP8266)
struct ESP_BoardConf {
	static constexpr size_t NUM_PINS = 40;
	static const ESP_PinMode PINOUT[NUM_PINS];
};
#endif