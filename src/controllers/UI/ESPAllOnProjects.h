#ifndef _ESPALLON_PROJECTS_H
#define _ESPALLON_PROJECTS_H

#include "../../utils.h"
#include "../ProjectsAPIClient.h"
#include <ESPUI.h>

#ifndef USE_LITTLEFS_MODE
#include "dataProjectsHTML.h"
#endif

#ifdef USE_LITTLEFS_MODE
#if defined(ESP32)
#if (ESP_IDF_VERSION_MAJOR == 4 && ESP_IDF_VERSION_MINOR >= 4) ||              \
	ESP_IDF_VERSION_MAJOR > 4
#include <LittleFS.h>
#else
#include <LITTLEFS.h>
#endif
#else
#include <LittleFS.h>
#endif
#endif
/**
 * Simple Projects endpoint handler for ESPAllOn system
 * Provides web interface for project management and configuration loading
 */
class ESPAllOnProjects {
  public:
	/**
	 * Register the projects endpoints with the web server
	 */
	static void registerProjectsEndpoints() {
		// Main projects page
		ESPUI.server->on("/projects", HTTP_GET, handleProjectsPageRequest);

		// API endpoints
		ESPUI.server->on("/api/projects", HTTP_GET, handleProjectsAPIRequest);
		ESPUI.server->on("/api/project/*/load", HTTP_POST,
						 handleLoadProjectRequest);

#ifdef USE_LITTLEFS_MODE
		// Serve CSS file for projects page when in LittleFS mode
		ESPUI.server->on(
			"/projects.css", HTTP_GET, [](AsyncWebServerRequest *request) {
#if defined(ESP32)
#if (ESP_IDF_VERSION_MAJOR == 4 && ESP_IDF_VERSION_MINOR >= 4) ||              \
	ESP_IDF_VERSION_MAJOR > 4
				request->send(LittleFS, "/projects.css", "text/css");
#else
			request->send(LITTLEFS, "/projects.css", "text/css");
#endif
#else
			request->send(LittleFS, "/projects.css", "text/css");
#endif
			});

		// Serve JavaScript file for projects page when in LittleFS mode
		ESPUI.server->on("/projects.js", HTTP_GET,
						 [](AsyncWebServerRequest *request) {
#if defined(ESP32)
#if (ESP_IDF_VERSION_MAJOR == 4 && ESP_IDF_VERSION_MINOR >= 4) ||              \
	ESP_IDF_VERSION_MAJOR > 4
							 request->send(LittleFS, "/projects.js",
										   "application/javascript");
#else
			request->send(LITTLEFS, "/projects.js", "application/javascript");
#endif
#else
			request->send(LittleFS, "/projects.js", "application/javascript");
#endif
						 });
#endif
		// Note: In embedded mode (USE_LITTLEFS_MODE disabled),
		// CSS and JS are included inline in the HTML

		DUMPSLN("Projects endpoints registered");
	}

  private:
	/**
	 * HTTP request handler for the projects page
	 * @param request AsyncWebServerRequest object
	 */
	static void handleProjectsPageRequest(AsyncWebServerRequest *request) {
		DUMPSLN("Serving projects page");

#ifdef USE_LITTLEFS_MODE
		// Serve HTML file from LittleFS
#if defined(ESP32)
#if (ESP_IDF_VERSION_MAJOR == 4 && ESP_IDF_VERSION_MINOR >= 4) ||              \
	ESP_IDF_VERSION_MAJOR > 4
		request->send(LittleFS, "/projects.html", "text/html");
#else
		request->send(LITTLEFS, "/projects.html", "text/html");
#endif
#else
		request->send(LittleFS, "/projects.html", "text/html");
#endif
#else
		// Serve embedded HTML page
		request->send_P(200, "text/html", PROJECTS_HTML);
#endif
	}

	/**
	 * HTTP request handler for the projects API endpoint
	 * @param request AsyncWebServerRequest object
	 */
	static void handleProjectsAPIRequest(AsyncWebServerRequest *request) {
		// Get pagination parameters from query string
		int page = 1;
		int limit = PROJECT_LIMIT_QUERY;

		if (request->hasParam("page")) {
			page = request->getParam("page")->value().toInt();
			if (page < 1)
				page = 1;
		}

		if (request->hasParam("limit")) {
			limit = request->getParam("limit")->value().toInt();
			if (limit < 1 || limit > 50)
				limit = PROJECT_LIMIT_QUERY; // Max 50 items per page
		}

		String jsonResponse =
			ProjectsAPIClient::getInstance().fetchProjectsJSON(page, limit);
		DUMPLN("API request for projects list (page=", page);
		DUMPLN(", limit=", limit);
		DUMPLN(") response: ", jsonResponse);

		if (jsonResponse.isEmpty()) {
			request->send(
				500, "application/json",
				"{\"success\":false,\"error\":\"Failed to fetch projects\"}");
			return;
		}

		request->send(200, "application/json", jsonResponse);
	}

	/**
	 * HTTP request handler for loading a project configuration
	 * @param request AsyncWebServerRequest object
	 */
	static void handleLoadProjectRequest(AsyncWebServerRequest *request) {
		String url = request->url();
		DUMPLN("Load project request: ", url);

		// Extract project ID from URL: /api/project/{id}/load
		int startIdx = url.indexOf("/api/project/") + 13;
		int endIdx = url.indexOf("/load");

		if (startIdx < 13 || endIdx == -1) {
			request->send(
				400, "application/json",
				"{\"success\":false,\"error\":\"Invalid project ID\"}");
			return;
		}

		String projectId = url.substring(startIdx, endIdx);
		DUMPLN("Extracted project ID: ", projectId);

		// TODO: Parse and apply the configuration to the ESP
		// This would involve:
		// 1. Parse the JSON configuration from request body
		// 2. Configure pins according to the project setup
		// 3. Initialize modules (steppers, sensors, etc.)
		// 4. Save configuration to EEPROM/Flash

		// For now, just return success
		request->send(200, "application/json",
					  "{\"success\":true,\"message\":\"Configuration loaded "
					  "successfully\"}");
	}
};

#endif
