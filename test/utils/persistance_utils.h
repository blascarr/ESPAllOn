#ifndef _ESP32_PERSISTANCE_UTILS_H
#define _ESP32_PERSISTANCE_UTILS_H
#define NVS
#include "nvs.h"
#include "nvs_flash.h"
#include <Persistance.h>

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

bool isThereAnyInfoInNVS(const String &key) { return existsKey(key); }

#define FILESYSTEM LittleFS

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

bool existsFileInLittleFS(String filename) {
	if (FILESYSTEM.exists(filename)) {
		return true;
	}
	return false;
}

bool deleteAll() {
	if (FILESYSTEM.begin()) {
		return FILESYSTEM.format();
	}
	return false;
}
#endif