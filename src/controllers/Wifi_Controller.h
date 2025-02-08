#ifndef _ESPALLON_WIFI_CONTROLLER_H
#define _ESPALLON_WIFI_CONTROLLER_H

#include "./ESPAllOn_Wifi.h"
#include <Arduino.h>
#include <DNSServer.h>
#include <EEPROM.h>

DNSServer dnsServer;

void enterWifiDetailsCallback(Control *sender, int type) {
	if (type == B_UP) {
		DUMPSLN("Saving credentials to EEPROM...");
		DUMPLN("SSID: ",
			   ESPUI.getControl(ESPALLON_Wifi::getInstance().wifi_ssid_text)
				   ->value);
		DUMPLN("PASS: ",
			   ESPUI.getControl(ESPALLON_Wifi::getInstance().wifi_pass_text)
				   ->value);
		unsigned int i;
		EEPROM.begin(100);
		for (i = 0;
			 i < ESPUI.getControl(ESPALLON_Wifi::getInstance().wifi_ssid_text)
					 ->value.length();
			 i++) {
			EEPROM.write(
				i,
				ESPUI.getControl(ESPALLON_Wifi::getInstance().wifi_ssid_text)
					->value.charAt(i));
			if (i == 30)
				break; // Even though we provided a max length, user input
					   // should never be trusted
		}
		EEPROM.write(i, '\0');

		for (i = 0;
			 i < ESPUI.getControl(ESPALLON_Wifi::getInstance().wifi_pass_text)
					 ->value.length();
			 i++) {
			EEPROM.write(
				i + 32,
				ESPUI.getControl(ESPALLON_Wifi::getInstance().wifi_pass_text)
					->value.charAt(i));
			if (i == 94)
				break; // Even though we provided a max length, user input
					   // should never be trusted
		}
		EEPROM.write(i + 32, '\0');
		EEPROM.end();
	}
}

#endif