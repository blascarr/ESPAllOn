#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <unity.h>

#include "../../config.h"

#include "../../../src/manager/ESPinner_Manager.h"
#include "../../utils/persistance_utils.h"

/**
 * Clear all WiFi credentials from EEPROM
 * Clears SSID (0-31) and Password (32-127) sections
 */
void clearWiFiCredentialsFromEEPROM() {
	EEPROM.begin(128);
	// Clear SSID section (0-31)
	for (int i = 0; i < 32; i++) {
		EEPROM.write(i, 0);
	}
	// Clear Password section (32-127)
	for (int i = 32; i < 128; i++) {
		EEPROM.write(i, 0);
	}
	EEPROM.commit();
	EEPROM.end();
	Serial.println("WiFi credentials cleared from EEPROM");
}

/**
 * Complete system reset - clears all persistent storage
 */
void completeStorageReset() {
	// Clear LittleFS (ESPinner configs and other files)
	bool littleFSCleared = deleteAll();
	Serial.println(littleFSCleared ? "LittleFS formatted successfully"
								   : "LittleFS format failed");

	// Clear WiFi credentials from EEPROM
	clearWiFiCredentialsFromEEPROM();

	// Clear ESPinner configs specifically (if LittleFS format wasn't enough)
	ESPinner_Manager::getInstance().clearPinConfigInStorage();
	ESPinner_Manager::getInstance().clearESPinners();

	Serial.println("Complete storage reset finished");
}

void test_is_there_any_file() {
	bool noFilesFound = isThereAnyFileInLittleFS();
	// isThereAnyFileInLittleFS returns true when NO files are found
	TEST_ASSERT_TRUE(noFilesFound);
}

void test_eeprom_cleared() {
	EEPROM.begin(128);
	bool allZeros = true;

	// Check SSID section (0-31)
	for (int i = 0; i < 32 && allZeros; i++) {
		if (EEPROM.read(i) != 0) {
			allZeros = false;
		}
	}

	// Check Password section (32-127)
	for (int i = 32; i < 128 && allZeros; i++) {
		if (EEPROM.read(i) != 0) {
			allZeros = false;
		}
	}

	EEPROM.end();
	TEST_ASSERT_TRUE(allZeros);
}

void setup() {
	Serial.begin(115200);
	UNITY_BEGIN();

	Serial.println("Starting complete storage reset test...");

	// Perform complete reset
	completeStorageReset();

	// Run tests to verify cleanup
	RUN_TEST(test_is_there_any_file);
	RUN_TEST(test_eeprom_cleared);

	UNITY_END();
}

void loop() {}