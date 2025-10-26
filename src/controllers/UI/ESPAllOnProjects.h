#ifndef _ESPALLON_PROJECTS_H
#define _ESPALLON_PROJECTS_H

#include "../../manager/ESPinner_Manager.h"
#include "../../utils.h"
#include "../ProjectsAPIClient.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>
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

		registerProjectPOSTEndpoints();

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
	 * HTTP request handler for POST endpoints
	 * /api/config/load
	 * @return void
	 */

	static void registerProjectPOSTEndpoints() {

		auto *jh = new AsyncCallbackJsonWebHandler(
			"/api/config/load",
			[](AsyncWebServerRequest *request, JsonVariant &json) {
				JsonObject obj = json.as<JsonObject>();

				// The JSON you send is the entire "project"; we access
				// obj["config"]
				if (!obj.containsKey("config")) {
					request->send(
						400, "application/json",
						R"({"success":false,"error":"Missing 'config'"})");
					return;
				}
				// We expect an array inside: obj["config"]["config"]
				if (!obj["config"].is<JsonArray>()) {
					request->send(
						400, "application/json",
						R"({"success":false,"error":"Invalid 'config' structure"})");
					return;
				}

				String configJson;
				serializeJson(obj["config"], configJson);
				DUMPLN("JSON converted to string: ", configJson);

				// Validate all pins before applying configuration
				String validationError;
				if (!validatePinsInConfig(obj["config"].as<JsonArray>(),
										  validationError)) {
					String errorResponse = "{\"success\":false,\"error\":\"Pin "
										   "validation failed: " +
										   validationError + "\"}";
					DUMPLN("Pin validation failed: ", validationError);
					request->send(400, "application/json", errorResponse);
					return;
				}

				bool ok =
					ESPinner_Manager::getInstance().loadFromJSON(configJson);

				if (ok)
					request->send(200, "application/json",
								  R"({"success":true})");
				else
					request->send(500, "application/json",
								  R"({"success":false})");
			});
		ESPUI.server->addHandler(jh);
	}
};

#endif
