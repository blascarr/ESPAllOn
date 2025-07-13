#include <Arduino.h>
#include <Wire.h>
#include <unity.h>

#include "../../config.h"

#include "../../../src/manager/ESPinner_Manager.h"
#include "../../utils/persistance_utils.h"

void test_is_there_any_file() {
	bool allFilesDeleted = isThereAnyFileInLittleFS();
	TEST_ASSERT_FALSE(allFilesDeleted);
}

void setup() {
	Serial.begin(115200);
	UNITY_BEGIN();
	isThereAnyFileInLittleFS();

	deleteAll();
	// RUN_TEST(test_is_there_any_file);

	UNITY_END();
}

void loop() {}