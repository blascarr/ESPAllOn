#include "ProjectsAPIClient.h"

// API Configuration
#define API_BASE_URL "https://espallon.blascarr.com"
#define API_PROJECTS_ENDPOINT "/api/espallones/projects"
#define API_PROJECT_ENDPOINT "/api/espallones/project/"

// Global instance reference
ProjectsAPIClient &projectsClient = ProjectsAPIClient::getInstance();

#if defined(ESP32)
WiFiClientSecure ProjectsAPIClient::setupSecureClient() {
	WiFiClientSecure client;
	// For development/testing, skip certificate verification
	client.setInsecure();
	return client;
}
#else
BearSSL::WiFiClientSecure ProjectsAPIClient::setupSecureClient() {
	BearSSL::WiFiClientSecure client;
	client.setInsecure();
	return client;
}
#endif

String ProjectsAPIClient::makeHTTPRequest(const String &url) {
	if (WiFi.status() != WL_CONNECTED) {
		Serial.println("WiFi not connected");
		return "";
	}

	auto client = setupSecureClient();
	HTTPClient http;

	Serial.print("Making HTTP request to: ");
	Serial.println(url);

	if (http.begin(client, url)) {

		Serial.println("Adding headers");
		http.addHeader("Content-Type", "application/json");
		http.addHeader("User-Agent", "ESPAllOn/1.0");

		int httpCode = http.GET();
		Serial.println(httpCode);
		if (httpCode > 0) {
			Serial.print("HTTP Response code: ");
			Serial.println(httpCode);

			if (httpCode == HTTP_CODE_OK) {
				String payload = http.getString();
				Serial.print("Response payload length: ");
				Serial.println(payload.length());
				http.end();
				return payload;
			} else {
				Serial.print("HTTP request failed with code: ");
				Serial.println(httpCode);
			}
		} else {
			Serial.print("HTTP request failed: ");
			Serial.println(http.errorToString(httpCode));
		}

		http.end();
	} else {
		Serial.println("Unable to connect to server");
	}

	return "";
}

String ProjectsAPIClient::fetchProjectsJSON() {
	String url = String(API_BASE_URL) + String(API_PROJECTS_ENDPOINT);

	return makeHTTPRequest(url);
}

String ProjectsAPIClient::fetchProjectConfigJSON(const String &projectId) {
	String url =
		String(API_BASE_URL) + String(API_PROJECT_ENDPOINT) + projectId;
	return makeHTTPRequest(url);
}

bool ProjectsAPIClient::isAPIAvailable() {
	String response = fetchProjectsJSON();
	return !response.isEmpty() && response.indexOf("\"success\":true") != -1;
}
