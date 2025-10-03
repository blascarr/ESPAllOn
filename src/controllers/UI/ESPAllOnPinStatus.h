#ifndef _ESPALLON_PIN_STATUS_H
#define _ESPALLON_PIN_STATUS_H

#include "../../utils.h"
#include "../ESPAllOnPinManager.h"
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
			  "<title>ESP Pin Status - ESPAllOn</title>"));

#ifdef USE_LITTLEFS_MODE
		// Use external CSS file when LittleFS is enabled
		res->print(F("<link rel='stylesheet' href='/pin-status.css'>"));
#else
		// Embed minimal CSS when LittleFS is disabled
		res->print(F(
			"<style>"
			":root{--bg:#f5f5f5;--card:#fff;--text:#333;--muted:#6c757d;--"
			"border:#ddd;--primary:#007bff;--success:#28a745;--info:#17a2b8;--"
			"danger:#dc3545;--warning:#ffc107;--even:#f8f9fa;--hover:#e9ecef}*{"
			"box-sizing:border-box}html,body{margin:0;padding:0;font-family:"
			"Arial,sans-serif;background:var(--bg);color:var(--text)}."
			"container{max-width:1200px;margin:0 "
			"auto;background:var(--card);padding:20px;border-radius:8px;box-"
			"shadow:0 2px 10px rgba(0,0,0,.1)}h1{text-align:center;margin:0 0 "
			"20px;color:var(--text)}.info-section{background:var(--hover);"
			"padding:15px;border-radius:5px;margin-bottom:20px}.info-section "
			"p{margin:5px "
			"0}table{width:100%;border-collapse:collapse;margin:20px "
			"0;font-size:14px}th,td{padding:8px "
			"12px;text-align:left;border:1px solid "
			"var(--border)}th{background:var(--muted);color:#fff;font-weight:"
			"700}tr:nth-child(even){background:var(--even)}tr:hover{background:"
			"var(--hover)}.pin-number{font-weight:700;text-align:center}."
			"status-used,.status-available,.status-broken,.status-not-"
			"configured,.mode-input,.mode-output,.mode-undefined{text-align:"
			"center;border-radius:3px}.status-used,.mode-output{background:var("
			"--success);color:#fff}.status-available{background:var(--info);"
			"color:#fff}.status-broken{background:var(--danger);color:#fff}."
			"status-not-configured{background:#87ceeb;color:#333}.mode-input{"
			"background:var(--warning);color:#212529}.mode-undefined{"
			"background:var(--muted);color:#fff}.flag-yes{color:var(--success);"
			"font-weight:700}.flag-no{color:var(--danger)}.pin-type{font-"
			"family:monospace;font-size:12px}.legend{margin:20px "
			"0;padding:15px;background:var(--even);border-radius:5px}.legend "
			"h3{margin:0 0 10px}.legend-item{display:inline-block;margin:5px "
			"10px}.legend-color{display:inline-block;width:20px;height:20px;"
			"margin-right:5px;vertical-align:middle;border-radius:3px}.legend-"
			"color.in-use{background:var(--success)}.legend-color.available{"
			"background:var(--info)}.legend-color.broken{background:var(--"
			"danger)}.legend-color.not-configured{background:#87ceeb}.actions{"
			"text-align:center;margin:20px 0}.actions button{padding:10px "
			"20px;margin:0 "
			"10px;background:var(--primary);color:#fff;border:0;border-radius:"
			"5px;cursor:pointer;font-size:16px}.actions "
			"button:hover{background:#0056b3}@media(max-width:768px){."
			"container{padding:10px}table{font-size:12px}th,td{padding:4px "
			"6px}}"
			"</style>"));
#endif

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