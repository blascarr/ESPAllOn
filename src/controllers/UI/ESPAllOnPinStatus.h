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

		// 1) Cabecera y CSS desde PROGMEM (no al heap)
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
		// timestamp
		{
			unsigned long s = millis() / 1000, m = s / 60, h = m / 60;
			res->printf(
				"<p><strong>Timestamp:</strong> Uptime: %luh %lum %lus</p>",
				h % 24, m % 60, s % 60);
		}
		res->print(F("</div>")); // info-section

		// 2) Tabla por filas (sin String grandes)
		res->print(F("<table><thead><tr>"
					 "<th>Pin #</th><th>GPIO</th><th>Status</th><th>Mode</th>"
					 "<th>Pin Type</th><th>Broken</th>"
#if defined(ESP32) || defined(ESP8266)
					 "<th>Deep Sleep</th><th>WiFi Safe</th><th>Touch GPIO</th>"
#endif
					 "<th>UI Ref</th></tr></thead><tbody>"));

		auto &pm = ESPAllOnPinManager::getInstance();
		for (uint8_t gpio = 0; gpio < ESP_BoardConf::NUM_PINS; ++gpio) {
			// busca config
			const ESP_PinMode *cfg = nullptr;
			for (size_t i = 0; i < (size_t)ESP_BoardConf::INITIAL_PINS; ++i) {
				if (ESP_BoardConf::PINOUT[i].pin == gpio) {
					cfg = &ESP_BoardConf::PINOUT[i];
					break;
				}
			}

			bool attached = pm.isPinAttached(gpio);
			bool broken = cfg ? cfg->isBroken : false;

			// uiRef (sin operator[] para no insertar claves y ahorrar heap)
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
			const char *brokenClass = broken ? "flag-yes" : "flag-no";

			const char *uiRefText =
				(uiRef == 0) ? "-" : (uiRef == 10000 ? "BLOCKED" : nullptr);

			res->printf("<tr><td class='pin-number'>%u</td><td "
						"class='pin-number'>%u</td>"
						"<td class='%s'>%s</td><td class='%s'>%s</td>"
						"<td class='pin-type'>%s</td>"
						"<td class='%s'>%s</td>",
						gpio, gpio, statusClass, statusText, modeClass,
						modeText, pinType, brokenClass, broken ? "YES" : "NO");

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
				res->print(
					F("<td class='flag-na'>N/A</td><td "
					  "class='flag-na'>N/A</td><td class='flag-na'>N/A</td>"));
			}
#endif
			if (uiRefText)
				res->printf("<td>%s</td></tr>", uiRefText);
			else
				res->printf("<td>%u</td></tr>", uiRef);
		}
		res->print(F("</tbody></table>"));

		// 3) Leyenda y acciones (también desde PROGMEM o F())
		res->print(F(
			"<div class='legend'>"
			"<h3>Legend</h3>"
			"<div class='legend-item'><span class='legend-color' "
			"style='background: #28a745;'></span>IN USE / USED</div>"
			"<div class='legend-item'><span class='legend-color' "
			"style='background: #17a2b8;'></span>AVAILABLE</div>"
			"<div class='legend-item'><span class='legend-color' "
			"style='background: #dc3545;'></span>BROKEN</div>"
			"<div class='legend-item'><span class='legend-color' "
			"style='background: #87ceeb;'></span>NOT CONFIGURED</div>"
			"</div>"
			"<div class='actions'><button "
			"onclick='location.reload()'>Refresh</button>"
			"<button onclick='location.href=\"/\"'>Back to Main</button></div>"
			"</div></body></html>"));

		request->send(res);
	}
};

#endif