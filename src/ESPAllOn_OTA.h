#include <Arduino.h>
#include <ArduinoOTA.h>
void OTA_update() { ArduinoOTA.handle(); }

Ticker OTA_UpdateTicker(OTA_update, 50, 0, MILLIS);

void OTA_init() {
	DUMPSLN("OTA INIT ");
	ArduinoOTA.setHostname(OTA_HOST);

	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH) {
			type = "sketch";
		} else { // U_FS
			type = "filesystem";
		}
		// NOTE: si redefinimos una partición de FS, usar esto para actualizar
		// SPIFFS
		DUMP("Start updating ", type);
	});

	ArduinoOTA.onEnd([]() { DUMPSLN("\nEnd"); });

	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		DUMP("Progress: ", (progress / (total / 100)));
	});

#if defined(OTA_PORT)
	ArduinoOTA.setPort(OTA_PORT);
#endif

#if defined(OTA_PASSWORD)
	ArduinoOTA.setPassword(OTA_PASSWORD);
#endif

#if defined(OTA_PASSWORD_HASH)
	ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
#endif
	ArduinoOTA.onError([](ota_error_t error) {
		DUMP("Error: ", error);
		if (error == OTA_AUTH_ERROR) {
			DUMPSLN("Auth Failed");
		} else if (error == OTA_BEGIN_ERROR) {
			DUMPSLN("Begin Failed");
		} else if (error == OTA_CONNECT_ERROR) {
			DUMPSLN("Connect Failed");
		} else if (error == OTA_RECEIVE_ERROR) {
			DUMPSLN("Receive Failed");
		} else if (error == OTA_END_ERROR) {
			DUMPSLN("End Failed");
		}
	});

	ArduinoOTA.begin();
}