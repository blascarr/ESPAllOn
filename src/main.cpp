
#include <Arduino.h>
#include <ESPUI.h>

#include <config.h>

#include "ESPinner.h"
#include "PinSerializable.h"
#include "Wifi_Controller.h"

#include "ESPAllOn.h"
#include <Ticker.h>

// Function Prototypes
void connectWifi();

pinSerializable datasource(10, InputPin(false, false), PinType::BusDigital);

// UI handles
ESPinner_GPIO gpio;
ESPinner_Stepper Esptepper;

void setup() {
	randomSeed(0);
	Serial.begin(115200);
	while (!Serial)
		;

	connectWifi();
#if defined(ESP32)
	WiFi.setSleep(false); // For the ESP32: turn off sleeping to increase UI
						  // responsivness (at the cost of power use)
#endif
	gpio.setup();
	gpio.loader();

	Esptepper.setup();
	Esptepper.loader();

	ESPALLON.setup();
	ESPALLON.begin();
	UI_UpdateTicker.start();
}

void loop() {

	UI_UpdateTicker.update();

	if (Serial.available()) {
		switch (Serial.read()) {
		case 'w': // Print IP details
			Serial.println(WiFi.localIP());
			break;
		case 'W': // Reconnect wifi
			connectWifi();
			break;
		case 'C': // Force a crash (for testing exception decoder)
#if !defined(ESP32)
			((void (*)())0xf00fdead)();
#endif
			break;
		default:
			Serial.print('#');
			break;
		}
	}

#if !defined(ESP32)
	// We don't need to call this explicitly on ESP32 but we do on 8266
	MDNS.update();
#endif
}