/**
 * Persistence Utilities for Testing
 *
 * This utility provides helper functions for testing persistent storage
 * functionality across different storage backends (NVS and LittleFS).
 *
 * Features:
 * - NVS (Non-Volatile Storage) key existence checking
 * - LittleFS file system validation and cleanup
 * - Storage backend abstraction for testing
 * - File existence validation utilities
 *
 * Storage Backends Supported:
 * - NVS: For ESP32/ESP8266 key-value storage
 * - LittleFS: For file-based storage operations
 */

#ifndef _ESP32_PERSISTANCE_UTILS_H
#define _ESP32_PERSISTANCE_UTILS_H
#define NVS

#include <Persistance.h>

/**
 * Check if a key exists in NVS storage
 * @param key The key to check for existence
 * @return True if key exists, false otherwise
 */
bool existsKey(const String &key) {
	String root = ESPinner_File;
	storage.storage.begin(root.c_str(), false);
	if (storage.storage.isKey(key.c_str())) {
		storage.storage.end();
		return true;
	}
	storage.storage.end();
	return false;
}

/*
void list_all_keys(const char *namespace) {
	nvs_handle_t my_handle;
	esp_err_t err = nvs_open(namespace, NVS_READONLY, &my_handle);
	if (err != ESP_OK) {
		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		nvs_iterator_t it = nvs_entry_find("nvs", namespace, NVS_TYPE_ANY);
		while (it != NULL) {
			nvs_entry_info_t info;
			nvs_entry_info(it, &info);
			it = nvs_entry_next(it);
			printf("key '%s', type '%d'\n", info.key, info.type);
		}
		nvs_release_iterator(it);
		nvs_close(my_handle);
	}
}
*/

/**
 * Check if any information exists in NVS for given key
 * @param key The key to check for data existence
 * @return True if data exists, false otherwise
 */
bool isThereAnyInfoInNVS(const String &key) { return existsKey(key); }

#define FILESYSTEM LittleFS

/**
 * Check if any files exist in LittleFS root directory
 * @return True if files are present, false if filesystem is empty
 */
bool isThereAnyFileInLittleFS() {
	File root = FILESYSTEM.open("/", "r");

	File file = root.openNextFile();
	while (file) {
		DUMP("File still present: ", file.name());
		file.close();
		return false;
	}
	file.close();
	root.close();
	return true;
}

/**
 * Check if a specific file exists in LittleFS
 * @param filename Name of file to check for existence
 * @return True if file exists, false otherwise
 */
bool existsFileInLittleFS(String filename) {
	if (FILESYSTEM.exists(filename)) {
		return true;
	}
	return false;
}

/**
 * Delete all files in LittleFS by formatting the filesystem
 * @return True if format successful, false on failure
 */
bool deleteAll() {
	if (FILESYSTEM.begin()) {
		return FILESYSTEM.format();
	}
	return false;
}
#endif