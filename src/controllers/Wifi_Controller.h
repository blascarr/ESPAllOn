#ifndef _ESPALLON_WIFI_CONTROLLER_H
#define _ESPALLON_WIFI_CONTROLLER_H

#include "./ESPAllOn_Wifi.h"
#include <Arduino.h>
#include <DNSServer.h>
#include <EEPROM.h>

/** DNS server instance for captive portal functionality */
DNSServer dnsServer;

/**
 * Helper function to get UI control reference by label
 * @param label The label to search for (SSID_LABEL or PASS_LABEL)
 * @return Control reference ID, or 0 if not found
 */
uint16_t getWiFiControlByLabel(const String &label) {
	auto it = WIFI_UI_ref.find(label);
	if (it != WIFI_UI_ref.end()) {
		return it->second;
	}
	return 0;
}

/**
 * Write string to EEPROM at specified address with length limit
 * @param data String data to write
 * @param address Starting address in EEPROM
 * @param maxLength Maximum characters to write (excluding null terminator)
 * @return Number of characters written (excluding null terminator)
 */
size_t writeStringToEEPROM(const String &data, int address, size_t maxLength) {
	size_t written = 0;
	size_t dataLength = data.length();

	// Write characters up to maxLength
	for (size_t i = 0; i < dataLength && i < maxLength; i++) {
		EEPROM.write(address + i, data.charAt(i));
		written++;
	}

	// Write null terminator
	EEPROM.write(address + written, '\0');

	return written;
}

/**
 * Clear EEPROM range by writing zeros
 * @param startAddress Starting address to clear
 * @param length Number of bytes to clear
 */
void clearEEPROMRange(int startAddress, size_t length) {
	for (size_t i = 0; i < length; i++) {
		EEPROM.write(startAddress + i, 0);
	}
}

/**
 * Validate IP address format (x.x.x.x where each x is 0-255)
 * @param ip String containing the IP address to validate
 * @return true if IP format is valid, false otherwise
 */
bool isValidIPFormat(const String &ip) {
	IPAddress addr;
	return addr.fromString(ip); // true si es IPv4 válida
}

/**
 * WiFi credentials structure for better organization
 */
struct WiFiCredentials {
	String ssid;
	String password;
	String localIp;
	String primaryDNS;
	String secondaryDNS;

	// EEPROM layout constants
	/**
	 * Validate WiFi credentials
	 * @return true if credentials are valid, false otherwise
	 */
	bool isValid() const {
		uint16_t save_button_ref = getWiFiControlByLabel(WIFI_SAVE_LABEL);
		char *dangerBackgroundStyle = getBackground(DANGER_COLOR);

		if (ssid.length() == 0 || ssid.length() > SSID_MAX_LENGTH) {
			DUMPSLN("ERROR: SSID is empty or too long!");
			ESPUI.setElementStyle(save_button_ref, dangerBackgroundStyle);
			return false;
		}

		if (password.length() == 0 || password.length() > PASS_MAX_LENGTH) {
			DUMPLN("ERROR: Password is empty or too long: ", password.length());
			ESPUI.setElementStyle(save_button_ref, dangerBackgroundStyle);
			return false;
		}

		char *successBackgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(save_button_ref, successBackgroundStyle);

		return true;
	}

	bool isValidIPDNS() const {
		uint16_t save_button_ref = getWiFiControlByLabel(IPDNS_SAVE_LABEL);
		char *dangerBackgroundStyle = getBackground(DANGER_COLOR);

		if (localIp.length() == 0 && primaryDNS.length() == 0 &&
			secondaryDNS.length() == 0) {
			DUMPSLN("ERROR: Data IP/DNS are empty!");
			ESPUI.setElementStyle(save_button_ref, dangerBackgroundStyle);
			return false;
		}

		if (localIp.length() == 0 || localIp.length() > IP_MAX_LENGTH) {
			DUMPSLN("ERROR: Local IP is empty or too long!");
		}
		if (primaryDNS.length() == 0 || primaryDNS.length() > IP_MAX_LENGTH) {
			DUMPSLN("ERROR: Primary DNS is empty or too long!");
		}
		if (secondaryDNS.length() == 0 ||
			secondaryDNS.length() > IP_MAX_LENGTH) {
			DUMPSLN("ERROR: Secondary DNS is empty or too long!");
		}

		DUMPSLN("SUCCESS: Data IP/DNS successfully validated!");
		char *successBackgroundStyle = getBackground(SUCCESS_COLOR);
		ESPUI.setElementStyle(save_button_ref, successBackgroundStyle);
		return true;
	}

	/**
	 * Save IP/DNS credentials to EEPROM
	 * @return true if saved successfully, false otherwise
	 */
	bool saveToEEPROMIPDNS() {
		bool success = false;
		EEPROM.begin(EEPROM_SIZE);
		if (localIp.length() != 0) {
			if (isValidIPFormat(localIp)) {
				clearEEPROMRange(LOCAL_IP_ADDRESS, IP_MAX_LENGTH);
				writeStringToEEPROM(localIp, LOCAL_IP_ADDRESS, IP_MAX_LENGTH);
				success = true;
				DUMPSLN("Local IP saved successfully!");
			} else {
				DUMPLN("ERROR: Local IP has invalid format: ", localIp);
			}
		}
		if (primaryDNS.length() != 0) {
			if (isValidIPFormat(primaryDNS)) {
				clearEEPROMRange(PRIMARY_DNS_ADDRESS, IP_MAX_LENGTH);
				writeStringToEEPROM(primaryDNS, PRIMARY_DNS_ADDRESS,
									IP_MAX_LENGTH);
				success = true;
				DUMPSLN("Primary DNS saved successfully!");
			} else {
				DUMPLN("ERROR: Primary DNS has invalid format: ", primaryDNS);
			}
		}

		if (secondaryDNS.length() != 0) {
			if (isValidIPFormat(secondaryDNS)) {
				clearEEPROMRange(SECONDARY_DNS_ADDRESS, IP_MAX_LENGTH);
				writeStringToEEPROM(secondaryDNS, SECONDARY_DNS_ADDRESS,
									IP_MAX_LENGTH);
				success = true;
				DUMPSLN("Secondary DNS saved successfully!");
			} else {
				DUMPLN("ERROR: Secondary DNS has invalid format: ",
					   secondaryDNS);
			}
		}
		EEPROM.end();
		return success;
	}

	/**
	 * Save credentials to EEPROM
	 * @return true if saved successfully, false otherwise
	 */
	bool saveToEEPROM() const {
		if (!isValid()) {
			return false;
		}
		EEPROM.begin(EEPROM_SIZE);

		// Clear previous data
		clearEEPROMRange(0, EEPROM_SIZE);

		// Save SSID
		size_t ssidWritten =
			writeStringToEEPROM(ssid, SSID_ADDRESS, SSID_MAX_LENGTH);

		// Save Password
		size_t passWritten =
			writeStringToEEPROM(password, PASS_ADDRESS, PASS_MAX_LENGTH);
		EEPROM.end();
		DUMPSLN("WiFi credentials saved successfully!");
		return true;
	}

	/**
	 * Print credentials info for debugging (password hidden)
	 */
	void debugPrint() const {
		DUMPLN("SSID: '", ssid + "'");
		DUMPLN("SSID Length: ", ssid.length());
		DUMPLN("PASS Length: ", password.length());
		DUMPLN("PASS (hidden): ", password.length() > 0 ? "****" : "EMPTY");
	}
};

/**
 * Callback for saving WiFi credentials to EEPROM
 * Saves SSID and password from UI controls to persistent storage
 */
void enterWifiDetailsCallback(Control *sender, int type) {
	if (type == B_UP) {
		// Get control references using the map
		uint16_t ssid_control_ref = getWiFiControlByLabel(SSID_LABEL);
		uint16_t pass_control_ref = getWiFiControlByLabel(PASS_LABEL);
		uint16_t save_button_ref = getWiFiControlByLabel(WIFI_SAVE_LABEL);
		uint16_t local_ip_control_ref = getWiFiControlByLabel(LOCAL_IP_LABEL);
		uint16_t dns_primary_control_ref =
			getWiFiControlByLabel(DNS_PRIMARY_LABEL);
		uint16_t dns_secondary_control_ref =
			getWiFiControlByLabel(DNS_SECONDARY_LABEL);

		if (ssid_control_ref == 0 || pass_control_ref == 0 ||
			save_button_ref == 0) {
			DUMPSLN("ERROR: WiFi controls not found in map!");
			return;
		}

		// Create WiFi credentials structure
		WiFiCredentials credentials;
		credentials.ssid = ESPUI.getControl(ssid_control_ref)->value;
		credentials.password = ESPUI.getControl(pass_control_ref)->value;

		// Check if credentials are valid and update button color
		if (credentials.isValid()) {
			// Valid credentials - try to save
			if (credentials.saveToEEPROM()) {
				DUMPSLN("Credentials saved! Scheduling reconnection...");
				ESPALLON_Wifi::getInstance().shouldReconnect = true;
			} else {
				DUMPSLN("Failed to save WiFi credentials!");
			}
		}
	}
}

void enterIPDNSDetailsCallback(Control *sender, int type) {
	if (type == B_UP) {
		uint16_t local_ip_control_ref = getWiFiControlByLabel(LOCAL_IP_LABEL);
		uint16_t dns_primary_control_ref =
			getWiFiControlByLabel(DNS_PRIMARY_LABEL);
		uint16_t dns_secondary_control_ref =
			getWiFiControlByLabel(DNS_SECONDARY_LABEL);
		uint16_t save_button_ref = getWiFiControlByLabel(IPDNS_SAVE_LABEL);

		// Create IP/DNS credentials structure
		WiFiCredentials credentials;
		credentials.localIp = ESPUI.getControl(local_ip_control_ref)->value;
		credentials.primaryDNS =
			ESPUI.getControl(dns_primary_control_ref)->value;
		credentials.secondaryDNS =
			ESPUI.getControl(dns_secondary_control_ref)->value;
		DUMPLN("Local IP Credentials: ", credentials.localIp);
		DUMPLN("Primary DNS Credentials: ", credentials.primaryDNS);
		DUMPLN("Secondary DNS Credentials: ", credentials.secondaryDNS);
		// Check if credentials are valid and update button color
		if (credentials.isValidIPDNS()) {
			// Valid credentials
			if (credentials.saveToEEPROMIPDNS()) {
				DUMPSLN("IP/DNS credentials saved! Scheduling "
						"reconnection...");
				ESPALLON_Wifi::getInstance().shouldReconnect = true;
			} else {
				DUMPSLN("Failed to save IP/DNS credentials!");
			}
		}
	}
}
#endif