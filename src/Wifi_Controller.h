#include <Arduino.h>
#include <DNSServer.h>
#include <EEPROM.h>

DNSServer dnsServer;

uint16_t wifi_ssid_text, wifi_pass_text;

void enterWifiDetailsCallback(Control *sender, int type);

void readStringFromEEPROM(String &buf, int baseaddress, int size) {
	buf.reserve(size);
	for (int i = baseaddress; i < baseaddress + size; i++) {
		char c = EEPROM.read(i);
		buf += c;
		if (!c)
			break;
	}
}

void connectWifi() {
	int connect_timeout;

#if defined(ESP32)
	WiFi.setHostname(HOSTNAME);
#else
	WiFi.hostname(HOSTNAME);
#endif
	DUMPSLN("Begin wifi...");

	// Load credentials from EEPROM
	if (!(FORCE_USE_HOTSPOT)) {
		String stored_ssid, stored_pass;
		if (!(HARDCODED_CREDENTIALS)) {
			yield();
			EEPROM.begin(100);

			readStringFromEEPROM(stored_ssid, 0, 32);
			readStringFromEEPROM(stored_pass, 32, 96);
			EEPROM.end();

		} else {
			stored_ssid = HARDCODED_SSID;
			stored_pass = HARDCODED_PASS;
		}

#if defined(DEBUG)
		DUMPLN("SSID: ", stored_ssid.c_str());
		DUMPLN("Pass: ", stored_pass.c_str());
#endif

		if (!WiFi.config(LOCAL_IP, GATEWAY, SUBNET)) {
			DUMPSLN("STA Failed to configure");
		}

#if defined(ESP32)
		WiFi.begin(stored_ssid.c_str(), stored_pass.c_str());
#else
		WiFi.begin(stored_ssid, stored_pass);
#endif
		connect_timeout = 28; // 7 seconds
		while (WiFi.status() != WL_CONNECTED && connect_timeout > 0) {
			delay(250);
			DUMPS(".");
			connect_timeout--;
		}
	}

	if (WiFi.status() == WL_CONNECTED) {
		DUMPPRINTLN();
		DUMPLN("IP: ", WiFi.localIP());

		if (!MDNS.begin(DNS_NAME)) {
			DUMPSLN("Error setting up MDNS responder!");
		}
	} else {
		DUMPSLN("\nCreating access point...");
		WiFi.mode(WIFI_AP);
		WiFi.softAPConfig(LOCAL_IP, GATEWAY, SUBNET);
		WiFi.softAP(HOSTNAME);

		connect_timeout = 20;
		do {
			delay(250);
			DUMPS(",");
			connect_timeout--;
		} while (connect_timeout);
	}
}

void textCallback(Control *sender, int type) {
	// This callback is needed to handle the changed values, even though it
	// doesn't do anything itself.
}

void enterWifiDetailsCallback(Control *sender, int type) {
	if (type == B_UP) {
		DUMPSLN("Saving credentials to EEPROM...");
		DUMPLN("SSID: ", ESPUI.getControl(wifi_ssid_text)->value);
		DUMPLN("PASS: ", ESPUI.getControl(wifi_pass_text)->value);
		unsigned int i;
		EEPROM.begin(100);
		for (i = 0; i < ESPUI.getControl(wifi_ssid_text)->value.length(); i++) {
			EEPROM.write(i, ESPUI.getControl(wifi_ssid_text)->value.charAt(i));
			if (i == 30)
				break; // Even though we provided a max length, user input
					   // should never be trusted
		}
		EEPROM.write(i, '\0');

		for (i = 0; i < ESPUI.getControl(wifi_pass_text)->value.length(); i++) {
			EEPROM.write(i + 32,
						 ESPUI.getControl(wifi_pass_text)->value.charAt(i));
			if (i == 94)
				break; // Even though we provided a max length, user input
					   // should never be trusted
		}
		EEPROM.write(i + 32, '\0');
		EEPROM.end();
	}
}
