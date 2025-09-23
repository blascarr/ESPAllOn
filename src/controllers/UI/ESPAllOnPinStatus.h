#ifndef _ESPALLON_PIN_STATUS_H
#define _ESPALLON_PIN_STATUS_H

#include "../../utils.h"
#include "../ESPAllOnPinManager.h"
#include <ESPAsyncWebServer.h>
#include <ESPUI.h>

/**
 * ESPAllOnPinStatus provides web endpoint functionality to display
 * comprehensive pin configuration and status information in a formatted
 * HTML table. It creates a dedicated web page accessible via /pin-status
 * that shows real-time pin usage, configuration, and board-specific details.
 */
class ESPAllOnPinStatus {
  public:
	/**
	 * Registers the pin status endpoint with the ESPUI web server
	 * This should be called after ESPUI.begin() to add the custom endpoint
	 */
	static void registerPinStatusEndpoint() {
		ESPUI.server->on("/pin-status", HTTP_GET, handlePinStatusRequest);
		DUMPLN("Pin Status endpoint registered at: ", "/pin-status");
	}

  private:
	/**
	 * HTTP request handler for the pin status endpoint
	 * @param request AsyncWebServerRequest object
	 */
	static void handlePinStatusRequest(AsyncWebServerRequest *request) {
		auto *res = request->beginResponseStream("text/html");

		// 1) Header and CSS from PROGMEM (no to heap)
		res->print(
			F("<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'>"
			  "<meta name='viewport' "
			  "content='width=device-width,initial-scale=1.0'>"
			  "<title>ESP Pin Status - ESPAllOn</title>"
			  "<link rel='stylesheet' href='/pin-status.css'>"));

		res->print(F("</head><body><div class='container'>"
					 "<h1>ESP Pin Configuration & Status</h1><div "
					 "class='info-section'>"));

#ifdef ESP32
		res->print(F("<p><strong>Board:</strong> ESP32</p>"));
#else
		res->print(F("<p><strong>Board:</strong> ESP8266</p>"));
#endif

		res->printf("<p><strong>Total Pins:</strong> %u</p>",
					(unsigned)ESP_BoardConf::NUM_PINS);
		res->print(F(
			"<div class='actions'><button "
			"onclick='location.reload()'>Refresh</button>"
			"<button onclick='location.href=\"/\"'>Back to Main</button></div>"
			"</div>")); // info-section

		// 2) Table by rows (no large String)
		res->print(F("<table><thead><tr>"
					 "<th>Pin #</th><th>GPIO</th><th>Status</th><th>Mode</th>"
					 "<th>Pin Type</th>"
#if defined(ESP32) || defined(ESP8266)
					 "<th>Deep Sleep</th><th>WiFi Safe</th><th>Touch GPIO</th>"
#endif
					 "<th>UI Ref</th></tr></thead><tbody>"));

		auto &pm = ESPAllOnPinManager::getInstance();
		for (uint8_t gpio = 0; gpio < ESP_BoardConf::NUM_PINS; ++gpio) {

			const ESP_PinMode *cfg = nullptr;
			for (size_t i = 0; i < (size_t)ESP_BoardConf::INITIAL_PINS; ++i) {
				if (ESP_BoardConf::PINOUT[i].pin == gpio) {
					cfg = &ESP_BoardConf::PINOUT[i];
					break;
				}
			}

			bool attached = pm.isPinAttached(gpio);
			bool broken = cfg ? cfg->isBroken : false;

			// uiRef (no operator[] to avoid inserting keys and save heap)
			uint16_t uiRef = 0;
			const auto &mapRef = pm.getPINMap();
			auto it = mapRef.find(gpio);
			if (it != mapRef.end())
				uiRef = it->second;

			const char *statusClass;
			const char *statusText;

			// Check if pin has UI reference (is being used in UI)
			bool hasUIRef = (uiRef != 0 && uiRef != 10000);

			if (!cfg) {
				if (hasUIRef) {
					statusClass = "status-used";
					statusText = "IN USE";
				} else {
					statusClass = "status-not-configured";
					statusText = "NOT CONFIGURED";
				}
			} else if (broken) {
				statusClass = "status-broken";
				statusText = "BROKEN";
			} else if (attached || hasUIRef) {
				statusClass = "status-used";
				statusText = hasUIRef ? "IN USE" : "USED";
			} else {
				statusClass = "status-available";
				statusText = "AVAILABLE";
			}

			const char *modeClass;
			const char *modeText;
			if (!cfg) {
				modeClass = "mode-undefined";
				modeText = "NOT CONFIGURED";
			} else
				switch (cfg->mode) {
				case GPIOMode::Input:
					modeClass = "mode-input";
					modeText = "INPUT";
					break;
				case GPIOMode::Output:
					modeClass = "mode-output";
					modeText = "OUTPUT";
					break;
				default:
					modeClass = "mode-undefined";
					modeText = "UNDEFINED";
				}

			const char *pinType = cfg ? getPinTypeName(cfg->type) : "None";

			const char *uiRefText =
				(uiRef == 0) ? "-" : (uiRef == 10000 ? "BLOCKED" : nullptr);

			res->printf("<tr><td class='pin-number'>%u</td><td "
						"class='pin-number'>%u</td>"
						"<td class='%s'>%s</td><td class='%s'>%s</td>"
						"<td class='pin-type'>%s</td>",
						gpio, gpio, statusClass, statusText, modeClass,
						modeText, pinType);

#if defined(ESP32) || defined(ESP8266)
			if (cfg) {
				res->printf("<td class='%s'>%s</td>",
							cfg->canDeepSleep ? "flag-yes" : "flag-no",
							cfg->canDeepSleep ? "YES" : "NO");
				res->printf("<td class='%s'>%s</td>",
							cfg->canUseWithWiFi ? "flag-yes" : "flag-no",
							cfg->canUseWithWiFi ? "YES" : "NO");
				res->printf("<td class='%s'>%s</td>",
							cfg->isTouchGPIO ? "flag-yes" : "flag-no",
							cfg->isTouchGPIO ? "YES" : "NO");
			} else {
				res->print(F(
					"<td class='flag-yes'>YES</td><td "
					"class='flag-yes'>YES</td><td class='flag-yes'>YES</td>"));
			}
#endif
			if (uiRefText)
				res->printf("<td>%s</td></tr>", uiRefText);
			else
				res->printf("<td>%u</td></tr>", uiRef);
		}
		res->print(F("</tbody></table>"));

		// 3) Legend and actions (also from PROGMEM or F())
		res->print(F("<div class='legend'>"
					 "<h3>Legend</h3>"
					 "<div class='legend-item'><span class='legend-color "
					 "in-use'></span>IN USE / USED</div>"
					 "<div class='legend-item'><span class='legend-color "
					 "available'></span>AVAILABLE</div>"
					 "<div class='legend-item'><span class='legend-color "
					 "broken'></span>BROKEN</div>"
					 "<div class='legend-item'><span class='legend-color "
					 "not-configured'></span>NOT CONFIGURED</div>"
					 "</div>"
					 "</div></body></html>"));

		request->send(res);
	}
};

#endif