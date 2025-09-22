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
	DUMPLN("WiFi control not found for label: ", label);
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
 * WiFi credentials structure for better organization
 */
struct WiFiCredentials {
	String ssid;
	String password;

	// EEPROM layout constants
	static const int SSID_ADDRESS = 0;
	static const int PASS_ADDRESS = 32;
	static const size_t SSID_MAX_LENGTH = 30; // Max SSID length for WPA
	static const size_t PASS_MAX_LENGTH = 63; // Max WPA password length
	static const size_t TOTAL_SIZE = 96;	  // Total EEPROM space used

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

	/**
	 * Save credentials to EEPROM
	 * @return true if saved successfully, false otherwise
	 */
	bool saveToEEPROM() const {
		if (!isValid()) {
			return false;
		}
		EEPROM.begin(TOTAL_SIZE);

		// Clear previous data
		clearEEPROMRange(0, TOTAL_SIZE);

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

#endif