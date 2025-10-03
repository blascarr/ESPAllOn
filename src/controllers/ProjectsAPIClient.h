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
	String fetchProjectsJSON();

	/**
	 * Fetch specific project configuration as JSON string
	 * @param projectId ID of the project to fetch
	 * @return JSON string with project configuration
	 */
	String fetchProjectConfigJSON(const String &projectId);

	/**
	 * Check if API is available
	 * @return True if API responds successfully
	 */
	bool isAPIAvailable();

  private:
	ProjectsAPIClient() {}

	/**
	 * Make HTTP GET request to specified URL
	 * @param url Full URL to request
	 * @return Response as String
	 */
	String makeHTTPRequest(const String &url);

	/**
	 * Setup WiFi client for HTTPS requests
	 */
#if defined(ESP32)
	WiFiClientSecure setupSecureClient();
#else
	BearSSL::WiFiClientSecure setupSecureClient();
#endif
};

// Global instance reference
extern ProjectsAPIClient &projectsClient;

#endif
