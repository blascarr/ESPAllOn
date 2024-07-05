
#include <Arduino.h>
#include <ESPUI.h>

#include "ESPinner.h"
#include "PinSerializable.h"
#include "Wifi_Controller.h"

#include "ESPAllOn.h"

// Function Prototypes
void connectWifi();

pinSerializable datasource(10, InputPin(false, false), PinType::BusDigital);

// UI handles
uint16_t wifi_ssid_text, wifi_pass_text;
ESPinner_GPIO gpio;
ESPinner_Stepper Esptepper;

void setup() {
	randomSeed(0);
	Serial.begin(115200);
	while (!Serial)
		;
	if (SLOW_BOOT)
		delay(5000); // Delay booting to give time to connect a serial monitor
	connectWifi();
#if defined(ESP32)
	WiFi.setSleep(false); // For the ESP32: turn off sleeping to increase UI
						  // responsivness (at the cost of power use)
#endif
	gpio.init();
	gpio.loader();

	Esptepper.init();
	Esptepper.loader();
	setUpUI();
}

void loop() {
	static long unsigned lastTime = 0;

	// Send periodic updates if switcher is turned on
	if (updates && millis() > lastTime + 500) {
		static uint16_t sliderVal = 10;

		ESPUI.updateLabel(mainLabel, String(sliderVal));
		lastTime = millis();
	}

	// Simple debug UART interface
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
		case 'm':
			ESPUI.addControl(Label, "New Label", "Dynamic Label Text",
							 Turquoise, maintab, generalCallback);
			Serial.println("Added a new label");
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