#include <Arduino.h>

/*
 *  WIFI_NONE_SLEEP
 *  WIFI_LIGHT_SLEEP
 *  WIFI_MODEM_SLEEP
 */
void setSleepMode(WiFiSleepType mode) { WiFi.setSleepMode(mode); }

void setSleepTime(uint16_t time) {
	// Time should be inserted in us ( microseconds )
	ESP.deepSleep(time);
}