#ifndef _ESPALLON_PIN_STATUS_H
#define _ESPALLON_PIN_STATUS_H

#include "../utils.h"
#include "ESPAllOnPinManager.h"
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
		ESPUI.server->on("/pin-status.css", HTTP_GET, handleCSSRequest);
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
			/* … items … */
			"</div>"
			"<div class='actions'><button "
			"onclick='location.reload()'>Refresh</button>"
			"<button onclick='location.href=\"/\"'>Back to Main</button></div>"
			"</div></body></html>"));

		request->send(res);
	}

	/**
	 * HTTP request handler for the CSS file
	 * @param request AsyncWebServerRequest object
	 */
	static void handleCSSRequest(AsyncWebServerRequest *request) {
		static const char CSS[] PROGMEM = R"rawliteral(
/* ========== Pin Status Styles ========== */
:root {
	--bg: #f5f5f5;
	--card: #ffffff;
	--text: #333333;
	--muted: #6c757d;
	--border: #dddddd;
	--primary: #007bff;
	--primary-dark: #0056b3;
	--success: #28a745;
	--info: #17a2b8;
	--danger: #dc3545;
	--warning: #ffc107;
	--even: #f8f9fa;
	--hover: #e9ecef;
}

* {
	box-sizing: border-box;
}

html,
body {
	margin: 0;
	padding: 0;
	font-family: Arial, Helvetica, sans-serif;
	background: var(--bg);
	color: var(--text);
}

.container {
	max-width: 1200px;
	margin: 0 auto;
	background: var(--card);
	padding: 20px;
	border-radius: 8px;
	box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
}

h1 {
	text-align: center;
	margin: 0 0 20px;
	color: var(--text);
}

.info-section {
	background: #e9ecef;
	padding: 15px;
	border-radius: 5px;
	margin-bottom: 20px;
}
.info-section p {
	margin: 5px 0;
}

table {
	width: 100%;
	border-collapse: collapse;
	margin: 20px 0;
	font-size: 14px;
}
th,
td {
	padding: 8px 12px;
	text-align: left;
	border: 1px solid var(--border);
}
th {
	background: var(--muted);
	color: #fff;
	font-weight: bold;
}
tr:nth-child(even) {
	background: var(--even);
}
tr:hover {
	background: var(--hover);
}

.pin-number {
	font-weight: bold;
	text-align: center;
}

.status-used {
	background: var(--success);
	color: #fff;
	text-align: center;
	border-radius: 3px;
}
.status-available {
	background: var(--info);
	color: #fff;
	text-align: center;
	border-radius: 3px;
}
.status-broken {
	background: var(--danger);
	color: #fff;
	text-align: center;
	border-radius: 3px;
}
.status-not-configured {
	background: #87ceeb;
	color: #333;
	text-align: center;
	border-radius: 3px;
}

.mode-input {
	background: var(--warning);
	color: #212529;
	text-align: center;
	border-radius: 3px;
}
.mode-output {
	background: var(--success);
	color: #fff;
	text-align: center;
	border-radius: 3px;
}
.mode-undefined {
	background: var(--muted);
	color: #fff;
	text-align: center;
	border-radius: 3px;
}

.flag-yes {
	color: var(--success);
	font-weight: bold;
}
.flag-no {
	color: var(--danger);
}
.flag-na {
	color: var(--muted);
	font-style: italic;
}

.pin-type {
	font-family: monospace;
	font-size: 12px;
}

.legend {
	margin: 20px 0;
	padding: 15px;
	background: var(--even);
	border-radius: 5px;
}
.legend h3 {
	margin: 0 0 10px;
}
.legend-item {
	display: inline-block;
	margin: 5px 10px;
}
.legend-color {
	display: inline-block;
	width: 20px;
	height: 20px;
	margin-right: 5px;
	vertical-align: middle;
	border-radius: 3px;
}

.actions {
	text-align: center;
	margin: 20px 0;
}
.actions button {
	padding: 10px 20px;
	margin: 0 10px;
	background: var(--primary);
	color: #fff;
	border: none;
	border-radius: 5px;
	cursor: pointer;
	font-size: 16px;
}
.actions button:hover {
	background: var(--primary-dark);
}

/* Responsive */
@media (max-width: 768px) {
	.container {
		padding: 10px;
	}
	table {
		font-size: 12px;
	}
	th,
	td {
		padding: 4px 6px;
	}
}

/* Auto dark mode */
@media (prefers-color-scheme: dark) {
	:root {
		--bg: #0f1115;
		--card: #151821;
		--text: #e5e7eb;
		--muted: #3b4252;
		--border: #2a2f3a;
		--even: #1a1f2b;
		--hover: #202636;
		--primary: #3b82f6;
		--primary-dark: #2563eb;
	}
	.info-section {
		background: #1f2432;
	}
}
)rawliteral";

		request->send_P(200, "text/css", CSS);
	}
};

#endif
