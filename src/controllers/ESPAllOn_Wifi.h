#ifndef _ESPALLON_WIFI_H
#define _ESPALLON_WIFI_H

#include "../config.h"
#include "./ESPAllOn_Controller.h"

#include <Arduino.h>
#include <DNSServer.h>
#include <EEPROM.h>

class WiFiInterface {
  public:
	virtual bool checkWifiConnection() = 0;
};

class ESPALLON_Wifi : public WiFiInterface, public ESPALLON_Controller {
  public:
	uint16_t wifi_ssid_text, wifi_pass_text;
	bool wifiStatus = false;

	ESPALLON_Wifi() {
		ESPALLON_Controller::setCallback(
			std::bind(&ESPALLON_Wifi::connectTick, this));
	}

	static ESPALLON_Wifi &getInstance() {
		static ESPALLON_Wifi instance;
		return instance;
	}

	void begin() override {
		wifiConfig();
		ESPALLON_Controller::start();
		wifiScan();
	}

	bool checkWifiConnection() override {
		if (WiFi.status() != WL_CONNECTED) {
			// DUMPSLN("Error: Board could not connect to WiFi.");
			wifiStatus = false;
		} else {
			// DUMPSLN("Wifi Connected");
			wifiStatus = true;
		}
		return wifiStatus;
	}

	void wifiConfig() {
		DUMPSLN("Wi-Fi ...");
		WiFi.mode(WIFI_STA);

		WiFi.begin(HARDCODED_SSID, HARDCODED_PASS);

		if (!WiFi.config(LOCAL_IP, GATEWAY, SUBNET, PRIMARYDNS, SECONDARYDNS)) {
			DUMPSLN("STA Failed to configure");
		}
	}

	void connectTick() override {
		if (ESPALLON_Controller::state() == RUNNING) {
			DUMPLN("Counter: ", counter());

			// 10 Times each second = 10 seconds
			if (ESPALLON_Controller::counter() >= 10 ||
				(WiFi.status() == WL_CONNECTED)) {
				DUMPLN("WIFI CONNECTED ", counter());
				ESPALLON_Controller::setCallback(
					std::bind(&ESPALLON_Wifi::checkWifiConnection, this));
			};
		}
	}

	void resetConnection() {
		ESPALLON_Controller::stop();
		wifiConfig();
		ESPALLON_Controller::setCallback(
			std::bind(&ESPALLON_Controller::connectTick, this));
		ESPALLON_Controller::start();
	}

	void getCurrentStatus() {}

	bool connected() {
		if (wifiStatus) {
			DUMPSLN("");
			DUMPLN("Wifi Connected - Local IP : ", WiFi.localIP());
			DUMPLN("GateWay IP = ", WiFi.gatewayIP());
			DUMPLN("SubnetMask = ", WiFi.subnetMask());
		}
		return wifiStatus;
	}

	void wifiScan() {
		int n = WiFi.scanNetworks();
		DUMPLN("Redes wifi:", String(n));
		for (int i = 0; i < n; ++i) {
			DUMP("-> ", i + 1);
			DUMP(" : ", WiFi.SSID(i));
			DUMP(" (", WiFi.RSSI(i));
			DUMPSLN(")");
		}
	}

	void disconnect() {
		WiFi.disconnect();
		checkWifiConnection();
	}

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
};

#endif