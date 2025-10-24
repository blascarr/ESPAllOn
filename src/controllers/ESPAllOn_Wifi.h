#ifndef _ESPALLON_WIFI_H
#define _ESPALLON_WIFI_H

#include "../config.h"
#include "./ESPAllOn_Controller.h"

#include <Arduino.h>
#include <DNSServer.h>
#include <EEPROM.h>

/**
 * Abstract interface for WiFi functionality
 */
class WiFiInterface {
  public:
	/** Check if WiFi connection is active */
	virtual bool checkWifiConnection() = 0;
};

/**
 * WiFi management class for ESPAllOn system
 * Handles WiFi connection, configuration, and status monitoring
 */
class ESPALLON_Wifi : public WiFiInterface, public ESPALLON_Controller {
  public:
	uint16_t wifi_ssid_text, wifi_pass_text, wifi_local_ip_text,
		wifi_dns_primary_text, wifi_dns_secondary_text;
	bool wifiStatus = false;
	bool shouldReconnect = false; // Flag for deferred reconnection

	/**
	 * Constructor sets up the connection tick callback
	 */
	ESPALLON_Wifi() {
		ESPALLON_Controller::setCallback(
			std::bind(&ESPALLON_Wifi::connectTick, this));
	}

	/**
	 * Gets the singleton instance of ESPALLON_Wifi
	 * @return Reference to the singleton instance
	 */
	static ESPALLON_Wifi &getInstance() {
		static ESPALLON_Wifi instance;
		return instance;
	}

	/**
	 * Initialize WiFi controller
	 * Configures WiFi, starts controller, and scans for networks
	 */
	void begin() override {
		wifiConfig();
		ESPALLON_Controller::interval(500);
		ESPALLON_Controller::start();
		wifiScan();
		connectWifi();
	}

	/**
	 * Check if WiFi connection is active
	 * @return True if connected, false otherwise
	 */
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

	/**
	 * Configure WiFi with hardcoded credentials
	 * Sets up station mode and network configuration
	 */
	void wifiConfig() {
		DUMPSLN("Wi-Fi ...");
		WiFi.mode(WIFI_STA);

		// Load credentials using centralized function
		String stored_ssid, stored_pass, stored_local_ip, stored_gateway,
			stored_subnet, stored_primary_dns, stored_secondary_dns;
		loadCredentials(stored_ssid, stored_pass, stored_local_ip,
						stored_gateway, stored_subnet, stored_primary_dns,
						stored_secondary_dns);

		// Configure WiFi
		IPAddress localIP, gateway, subnet, primaryDNS, secondaryDNS;
		WiFi.begin(stored_ssid, stored_pass);

		// Parse IP addresses correctly
		localIP.fromString(stored_local_ip);
		gateway.fromString(stored_gateway);
		subnet.fromString(stored_subnet);
		primaryDNS.fromString(stored_primary_dns);
		secondaryDNS.fromString(stored_secondary_dns);

		DUMPLN("Configuring WiFi with IP: ", localIP);

		if (!WiFi.config(localIP, gateway, subnet, primaryDNS, secondaryDNS)) {
			DUMPSLN("STA Failed to configure");
		} else {
			DUMPSLN("WiFi configured successfully");
			DUMPLN("Primary DNS: ", primaryDNS);
			DUMPLN("Secondary DNS: ", secondaryDNS);
		}
	}

	/**
	 * Connection tick handler
	 * Monitors connection attempts and switches to status when connected
	 */
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
		handleDeferredReconnection();
	}

	/**
	 * Reset WiFi connection
	 * Stops controller, reconfigures WiFi, and restarts
	 */
	void resetConnection() {
		ESPALLON_Controller::stop();
		wifiConfig();
		ESPALLON_Controller::setCallback(
			std::bind(&ESPALLON_Controller::connectTick, this));
		ESPALLON_Controller::start();
	}

	/** Get current status - placeholder function */
	void getCurrentStatus() {}

	/**
	 * Handle deferred reconnection requests
	 * Should be called in main loop to safely reconnect
	 */
	void handleDeferredReconnection() {
		if (shouldReconnect) {
			shouldReconnect = false;
			DUMPSLN("Performing deferred WiFi reconnection...");
			delay(2000); // Give time for web response to be sent
			connectWifi();
		}
	}

	/**
	 * Check if connected and print network information
	 * @return Current WiFi status
	 */
	bool connected() {
		if (wifiStatus) {
			DUMPSLN("");
			DUMPLN("Wifi Connected - Local IP : ", WiFi.localIP());
			DUMPLN("GateWay IP = ", WiFi.gatewayIP());
			DUMPLN("SubnetMask = ", WiFi.subnetMask());
		}
		return wifiStatus;
	}

	/**
	 * Scan for available WiFi networks
	 * Prints list of found networks with signal strength
	 */
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

	/**
	 * Disconnect from WiFi and update status
	 */
	void disconnect() {
		WiFi.disconnect();
		checkWifiConnection();
	}

	/**
	 * Read string from EEPROM
	 * @param buf Buffer to store the string
	 * @param baseaddress Starting address in EEPROM
	 * @param size Maximum size to read
	 */
	void readStringFromEEPROM(String &buf, int baseaddress, int size) {
		buf.reserve(size);
		for (int i = baseaddress; i < baseaddress + size; i++) {
			char c = EEPROM.read(i);
			buf += c;
			if (!c)
				break;
		}
	}

	/**
	 * Load WiFi credentials from EEPROM or hardcoded values
	 * @param ssid Reference to store SSID
	 * @param password Reference to store password
	 * @param localIp Reference to store local IP
	 * @param gateway Reference to store gateway
	 * @param subnet Reference to store subnet
	 * @param primaryDns Reference to store primary DNS
	 * @param secondaryDns Reference to store secondary DNS
	 */
	void loadCredentials(String &ssid, String &password, String &localIp,
						 String &gateway, String &subnet, String &primaryDns,
						 String &secondaryDns) {
		if (!(HARDCODED_CREDENTIALS)) {
			// Load from EEPROM
			yield();
			EEPROM.begin(EEPROM_SIZE);

			readStringFromEEPROM(ssid, 0, 32);
			readStringFromEEPROM(password, 32, 96);
			readStringFromEEPROM(localIp, 64, 32);
			readStringFromEEPROM(primaryDns, 96, 32);
			readStringFromEEPROM(secondaryDns, 128, 32);

			// Gateway and subnet are always hardcoded
			gateway = HARDCODED_GATEWAY;
			subnet = HARDCODED_SUBNET;

			EEPROM.end();
			DUMPSLN("Credentials loaded from EEPROM");
		} else {
			// Use hardcoded values
			ssid = HARDCODED_SSID;
			password = HARDCODED_PASS;
			localIp = HARDCODED_LOCAL_IP;
			primaryDns = HARDCODED_PRIMARYDNS;
			secondaryDns = HARDCODED_SECONDARYDNS;
			gateway = HARDCODED_GATEWAY;
			subnet = HARDCODED_SUBNET;
			DUMPSLN("Using hardcoded credentials");
		}

		// Debug print
		DUMPLN("Loaded SSID: ", ssid);
		DUMPLN("Loaded Local IP: ", localIp);
		DUMPLN("Loaded Primary DNS: ", primaryDns);
		DUMPLN("Loaded Secondary DNS: ", secondaryDns);
	}

	/**
	 * Connect to WiFi network
	 * Attempts connection with stored/hardcoded credentials or creates AP
	 */
	void connectWifi() {
		int connect_timeout;

#if defined(ESP32)
		WiFi.setHostname(HOSTNAME);
#else
		WiFi.hostname(HOSTNAME);
#endif
		DUMPSLN("Begin wifi...");

		// Try to connect to WiFi if not forced to use hotspot
		if (!(FORCE_USE_HOTSPOT)) {
			// Load credentials using centralized function
			String stored_ssid, stored_pass, stored_local_ip, stored_gateway,
				stored_subnet, stored_primary_dns, stored_secondary_dns;
			loadCredentials(stored_ssid, stored_pass, stored_local_ip,
							stored_gateway, stored_subnet, stored_primary_dns,
							stored_secondary_dns);

#if defined(DEBUG)
			DUMPLN("SSID: ", stored_ssid.c_str());
			DUMPLN("Pass: ", stored_pass.c_str());
#endif

			// Configure network settings
			IPAddress localIP, gateway, subnet, primaryDNS, secondaryDNS;
			localIP.fromString(stored_local_ip);
			gateway.fromString(stored_gateway);
			subnet.fromString(stored_subnet);
			primaryDNS.fromString(stored_primary_dns);
			secondaryDNS.fromString(stored_secondary_dns);

			DUMPLN("Network configured - IP: ", localIP);

			if (!WiFi.config(localIP, gateway, subnet, primaryDNS,
							 secondaryDNS)) {
				DUMPSLN("STA Failed to configure");
			} else {
				DUMPLN("DNS configured - Primary: ", primaryDNS);
				DUMPLN("DNS configured - Secondary: ", secondaryDNS);
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
			// Create Access Point if connection failed
			DUMPSLN("\nCreating access point...");

			// Load credentials using centralized function
			String stored_ssid, stored_pass, stored_local_ip, stored_gateway,
				stored_subnet, stored_primary_dns, stored_secondary_dns;
			loadCredentials(stored_ssid, stored_pass, stored_local_ip,
							stored_gateway, stored_subnet, stored_primary_dns,
							stored_secondary_dns);

			// Configure Access Point
			IPAddress localIP, gateway, subnet;
			localIP.fromString(stored_local_ip);
			gateway.fromString(stored_gateway);
			subnet.fromString(stored_subnet);

			DUMPLN("Creating AP with IP: ", localIP);

			WiFi.mode(WIFI_AP);
			WiFi.softAPConfig(localIP, gateway, subnet);
			WiFi.softAP(HOSTNAME);
			DUMPLN("AP created with SSID: ", HOSTNAME);

			// Start DNS server for captive portal
			setupCaptivePortal();

			connect_timeout = 20;
			do {
				delay(250);
				DUMPS(",");
				connect_timeout--;
			} while (connect_timeout);
		}
	}

	/**
	 * Setup captive portal for WiFi configuration
	 * Configures DNS server to redirect all requests to the ESP
	 */
	void setupCaptivePortal() {
		// Include DNS server from Wifi_Controller
		extern DNSServer dnsServer;

		// Start DNS server to redirect all requests to our IP
		dnsServer.start(DNS_PORT, "*", DNS_PORT);
		DUMPSLN("Captive portal DNS server started");
	}
};

#endif