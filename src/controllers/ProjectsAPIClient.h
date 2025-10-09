#ifndef _PROJECTS_API_CLIENT_H
#define _PROJECTS_API_CLIENT_H

#include <Arduino.h>
#include <ArduinoJson.h>

#if defined(ESP32)
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#else
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#endif

/**
 * Simple Projects API client for fetching project data
 */
class ProjectsAPIClient {
  public:
	/**
	 * Gets the singleton instance
	 */
	static ProjectsAPIClient &getInstance() {
		static ProjectsAPIClient instance;
		return instance;
	}

	/**
	 * Fetch projects list as JSON string
	 * @return JSON string with projects data
	 */
	String fetchProjectsJSON() {
		String url = String(API_BASE_URL) + String(API_PROJECTS_ENDPOINT);

		return makeHTTPRequest(url);
	};

	/**
	 * Fetch specific project configuration as JSON string
	 * @param projectId ID of the project to fetch
	 * @return JSON string with project configuration
	 */
	String fetchProjectConfigJSON(const String &projectId) {
		String url =
			String(API_BASE_URL) + String(API_PROJECT_ENDPOINT) + projectId;
		return makeHTTPRequest(url);
	};

	/**
	 * Check if API is available
	 * @return True if API responds successfully
	 */
	bool isAPIAvailable() {
		String response = fetchProjectsJSON();
		return !response.isEmpty() &&
			   response.indexOf("\"success\":true") != -1;
	};

  private:
	ProjectsAPIClient() {}

	/**
	 * Make HTTP GET request to specified URL
	 * @param url Full URL to request
	 * @return Response as String
	 */
	String makeHTTPRequest(const String &url) {
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
	};

	/**
	 * Setup WiFi client for HTTPS requests
	 */
#if defined(ESP32)
	WiFiClientSecure setupSecureClient() {
		WiFiClientSecure client;
		// For development/testing, skip certificate verification
		client.setInsecure();
		return client;
	};
#else
	BearSSL::WiFiClientSecure setupSecureClient() {
		BearSSL::WiFiClientSecure client;
		client.setInsecure();
		return client;
	};
#endif
};

// Global instance reference
// extern ProjectsAPIClient &projectsClient;

#endif
