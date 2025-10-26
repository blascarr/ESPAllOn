/**
 * Endpoint Unit Test
 *
 * This test validates the HTTP endpoint for loading ESPinner configuration
 * from JSON format. It makes real HTTP requests to the /api/config/load
 * endpoint and verifies the response and data processing.
 *
 * Test Steps:
 * 1. Establish WiFi connection
 * 2. Start web server
 * 3. Send HTTP POST request to /api/config/load with JSON configuration
 * 4. Verify HTTP response is successful
 * 5. Verify ESPinners are loaded correctly
 */

#include "../../../src/config.h"

#include "../../config.h"
#include "../../utils/dump_utils.h"

#include "../../../src/controllers/ESPAllOn_Wifi.h"
#include "../../../src/controllers/Wifi_Controller.h"
#include "../../../src/mods/ESPinner_Stepper/ESPinner_Stepper.h"
#include "../../utils/testTicker.h"

#include <HTTPClient.h>

// Test configuration JSON with two stepper motors
// This matches the format received by the endpoint
String testConfigJson = R"({
	"config": {
		"config": [
			{
				"DIR": 3,
				"DRIVER": "ACCELSTEPPER",
				"EN": 4,
				"ESPinner_Mod": "ESPINNER_STEPPER",
				"ID": "MAIN_STEPPER",
				"STEP": 2,
				"STEPS_PER_REV": 200
			},
			{
				"DIR": 16,
				"DRIVER": "ACCELSTEPPER",
				"EN": 17,
				"ESPinner_Mod": "ESPINNER_STEPPER",
				"ID": "SECONDARY_STEPPER",
				"STEP": 18,
				"STEPS_PER_REV": 400
			}
		]
	}
})";

void runEndpointsTest() {
	if (currentTestIndex < testFunctions.size()) {
		testFunctions[currentTestIndex](); // Execute Current Test
		currentTestIndex++;				   // Next Index
	} else {
		DUMPSLN("All tests completed");
		ESPALLON_Wifi::getInstance().stop();
		UNITY_END();
	}
}

TickerFree<> test_runTicker(runEndpointsTest, 3000, 0, MILLIS);

/**
 * Test WiFi disconnected state
 * Validates that WiFi is initially disconnected
 */
void test_nowifi() {
	bool WifiConnected = ESPALLON_Wifi::getInstance().checkWifiConnection();
	TEST_ASSERT_FALSE(WifiConnected);
}

/**
 * Test WiFi connection establishment
 * Validates that WiFi connection is successfully established
 */
void test_wifi() {
	bool WifiConnected = ESPALLON_Wifi::getInstance().checkWifiConnection();
	TEST_ASSERT_TRUE_MESSAGE(WifiConnected, "WiFi should be connected");
}

/**
 * Test HTTP POST request to /api/config/load endpoint
 * Sends JSON configuration and verifies the response
 */
void test_endpoint_config_load() {
	DUMPSLN("Testing /api/config/load endpoint");

	// Clear any existing ESPinners
	ESPinner_Manager::getInstance().clearESPinners();

	// Get local IP address
	String localIP = WiFi.localIP().toString();
	String url = "http://" + localIP + "/api/config/load";

	DUMPLN("Sending POST request to: ", url);
	DUMPLN("Request body: ", testConfigJson);

	HTTPClient http;
	http.begin(url);
	http.addHeader("Content-Type", "application/json");

	// Send POST request with JSON configuration
	int httpResponseCode = http.POST(testConfigJson);

	DUMPLN("HTTP Response code: ", httpResponseCode);

	// Verify HTTP response is successful (200 OK)
	TEST_ASSERT_EQUAL_INT_MESSAGE(200, httpResponseCode,
								  "HTTP response should be 200 OK");

	// Get response payload
	String response = http.getString();
	DUMPLN("Response: ", response);

	http.end();

	// Parse response JSON
	JsonDocument doc;
	DeserializationError error = deserializeJson(doc, response);

	// Verify response indicates success
	TEST_ASSERT_TRUE_MESSAGE(doc["success"].as<bool>(),
							 "Response should indicate success");

	// Small delay to ensure configuration is processed
	delay(500);

	// Verify ESPinners were loaded

	size_t espinnerCount = ESPinner_Manager::getInstance().espinnerSize();
	TEST_ASSERT_EQUAL_UINT32_MESSAGE(2, espinnerCount,
									 "Should have loaded 2 ESPinners");

	// Verify MAIN_STEPPER was loaded correctly
	ESPinner *mainStepper =
		ESPinner_Manager::getInstance().findESPinnerById("MAIN_STEPPER");
	TEST_ASSERT_NOT_NULL_MESSAGE(mainStepper, "MAIN_STEPPER should be found");

	ESPinner_Stepper *mainStepperCast =
		static_cast<ESPinner_Stepper *>(mainStepper);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(2, mainStepperCast->getSTEP(),
									"MAIN_STEPPER STEP pin should be 2");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(3, mainStepperCast->getDIR(),
									"MAIN_STEPPER DIR pin should be 3");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(4, mainStepperCast->getEN(),
									"MAIN_STEPPER EN pin should be 4");

	// Verify SECONDARY_STEPPER was loaded correctly
	ESPinner *secondaryStepper =
		ESPinner_Manager::getInstance().findESPinnerById("SECONDARY_STEPPER");
	TEST_ASSERT_NOT_NULL_MESSAGE(secondaryStepper,
								 "SECONDARY_STEPPER should be found");

	ESPinner_Stepper *secondaryStepperCast =
		static_cast<ESPinner_Stepper *>(secondaryStepper);
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(16, secondaryStepperCast->getDIR(),
									"SECONDARY_STEPPER DIR pin should be 6");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(17, secondaryStepperCast->getEN(),
									"SECONDARY_STEPPER EN pin should be 7");
	TEST_ASSERT_EQUAL_UINT8_MESSAGE(18, secondaryStepperCast->getSTEP(),
									"SECONDARY_STEPPER STEP pin should be 5");
	DUMPSLN("Endpoint test completed successfully");
}

/**
 * Test invalid JSON format to endpoint
 * Verifies that malformed JSON is rejected with proper error response
 */
void test_endpoint_invalid_json() {
	DUMPSLN("Testing /api/config/load with invalid JSON");

	String invalidJson = R"({invalid json})";

	String localIP = WiFi.localIP().toString();
	String url = "http://" + localIP + "/api/config/load";

	HTTPClient http;
	http.begin(url);
	http.addHeader("Content-Type", "application/json");

	int httpResponseCode = http.POST(invalidJson);

	DUMPLN("HTTP Response code: ", httpResponseCode);

	// Should return 400 Bad Request for invalid JSON
	TEST_ASSERT_EQUAL_INT_MESSAGE(
		400, httpResponseCode,
		"HTTP response should be 400 Bad Request for invalid JSON");

	String response = http.getString();
	DUMPLN("Response: ", response);

	http.end();

	// Parse response
	JsonDocument doc;
	deserializeJson(doc, response);

	// Verify response indicates failure
	TEST_ASSERT_FALSE_MESSAGE(doc["success"].as<bool>(),
							  "Response should indicate failure");

	DUMPSLN("Invalid JSON test completed");
}

void isWifiConnected() { RUN_TEST(test_wifi); }
void testConfigLoadEndpoint() { RUN_TEST(test_endpoint_config_load); }
void testInvalidJsonEndpoint() { RUN_TEST(test_endpoint_invalid_json); }

void setup() {
	Serial.begin(115200);
	delay(2000); // Wait for serial to initialize

	UNITY_BEGIN();

	Serial.println("Starting Endpoint Tests...");

	// Test 1: Verify WiFi is initially disconnected
	RUN_TEST(test_nowifi);

	// Initialize WiFi and connect
	ESPALLON_Wifi::getInstance().begin();

	// Setup and start ESPAllOn server
	ESPAllOn::getInstance().setup();
	ESPAllOn::getInstance().begin();
	testFunctions.push_back(isWifiConnected);

	// Add tests to be executed after WiFi connection

	testFunctions.push_back(testConfigLoadEndpoint);
	testFunctions.push_back(testInvalidJsonEndpoint);

	// Start WiFi connection and test ticker
	ESPALLON_Wifi::getInstance().start();
	test_runTicker.start();
}

void loop() { test_runTicker.update(); }
