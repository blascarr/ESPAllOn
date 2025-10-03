#ifndef _ESPALLON_PROJECTS_API_H
#define _ESPALLON_PROJECTS_API_H

#include "../config.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

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
 * Structure to represent a project from the API
 */
struct Project {
	String id;
	String name;
	String description;
	String status;
	String created_at;
	String updated_at;
	JsonDocument config;

	Project() { config.to<JsonObject>(); }

	Project(const Project &other) {
		id = other.id;
		name = other.name;
		description = other.description;
		status = other.status;
		created_at = other.created_at;
		updated_at = other.updated_at;
		config.set(other.config);
	}

	Project &operator=(const Project &other) {
		if (this != &other) {
			id = other.id;
			name = other.name;
			description = other.description;
			status = other.status;
			created_at = other.created_at;
			updated_at = other.updated_at;
			config.set(other.config);
		}
		return *this;
	}
};

/**
 * Projects API client class for ESPAllOn system
 * Handles communication with the ESPAllOn projects API
 */
class ESPAllOn_ProjectsAPI {
  public:
	/**
	 * Gets the singleton instance of ESPAllOn_ProjectsAPI
	 * @return Reference to the singleton instance
	 */
	static ESPAllOn_ProjectsAPI &getInstance() {
		static ESPAllOn_ProjectsAPI instance;
		return instance;
	}

	/**
	 * Fetch all projects from the API
	 * @return Vector of Project structures
	 */
	std::vector<Project> fetchProjects();

	/**
	 * Fetch a specific project configuration by ID
	 * @param projectId ID of the project to fetch
	 * @return Project structure with configuration
	 */
	Project fetchProjectConfig(const String &projectId);

	/**
	 * Apply project configuration to the current system
	 * @param project Project to apply configuration from
	 * @return True if configuration was applied successfully
	 */
	bool applyProjectConfiguration(const Project &project);

	/**
	 * Check if API is available
	 * @return True if API responds successfully
	 */
	bool isAPIAvailable();

  private:
	/**
	 * Constructor - private for singleton pattern
	 */
	ESPAllOn_ProjectsAPI() {}

	/**
	 * Make HTTP GET request to specified URL
	 * @param url Full URL to request
	 * @return JSON response as JsonDocument
	 */
	JsonDocument makeHTTPRequest(const String &url);

	/**
	 * Parse projects list from JSON response
	 * @param jsonDoc JSON document containing projects data
	 * @return Vector of Project structures
	 */
	std::vector<Project> parseProjectsList(const JsonDocument &jsonDoc);

	/**
	 * Parse single project from JSON response
	 * @param jsonDoc JSON document containing project data
	 * @return Project structure
	 */
	Project parseProject(const JsonDocument &jsonDoc);

	/**
	 * Setup WiFi client for HTTPS requests
	 * @return Configured WiFiClientSecure instance
	 */
#if defined(ESP32)
	WiFiClientSecure setupSecureClient();
#else
	BearSSL::WiFiClientSecure setupSecureClient();
#endif
};

// Global instance reference for easy access
extern ESPAllOn_ProjectsAPI &projectsAPI;

#endif
