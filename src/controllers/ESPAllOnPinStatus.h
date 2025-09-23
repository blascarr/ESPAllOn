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
		DUMPLN("Pin Status endpoint registered at: ", "/pin-status");
	}

	/**
	 * Generates the complete HTML page with pin status information
	 * @return String containing the full HTML page
	 */
	static String generatePinStatusHTML() {
		String html = getHTMLHeader();
		html += getTableCSS();
		html += "<body>";
		html += "<div class='container'>";
		html += "<h1>ESP Pin Configuration & Status</h1>";
		html += "<div class='info-section'>";
		html += "<p><strong>Board:</strong> ";
#if defined(ESP32)
		html += "ESP32";
#else
		html += "ESP8266";
#endif
		html += "</p>";
		html += "<p><strong>Total Pins:</strong> " +
				String(ESP_BoardConf::NUM_PINS) + "</p>";
		html += "<p><strong>Timestamp:</strong> " + getTimestamp() + "</p>";
		html += "</div>";
		html += generatePinTable();
		html += getTableLegend();
		html += "<div class='actions'>";
		html += "<button onclick='location.reload()'>Refresh</button>";
		html += "<button onclick='location.href=\"/\"'>Back to Main</button>";
		html += "</div>";
		html += "</div>";
		html += "</body></html>";
		return html;
	}

  private:
	/**
	 * HTTP request handler for the pin status endpoint
	 * @param request AsyncWebServerRequest object
	 */
	static void handlePinStatusRequest(AsyncWebServerRequest *request) {
		// Note: Authentication is handled by ESPUI internally for other
		// endpoints For now, this endpoint is publicly accessible
		// TODO: Add authentication check when ESPUI exposes auth methods
		// publicly

		String html = generatePinStatusHTML();
		request->send(200, "text/html", html);
		DUMPSLN("Pin status page served");
	}

	/**
	 * Generates the HTML header with metadata
	 * @return String containing HTML header
	 */
	static String getHTMLHeader() {
		return "<!DOCTYPE html>"
			   "<html lang='en'>"
			   "<head>"
			   "<meta charset='UTF-8'>"
			   "<meta name='viewport' content='width=device-width, "
			   "initial-scale=1.0'>"
			   "<title>ESP Pin Status - ESPAllOn</title>";
	}

	/**
	 * Generates CSS styles for the pin status table
	 * @return String containing CSS styles
	 */
	static String getTableCSS() {
		return "<style>"
			   "body { font-family: Arial, sans-serif; margin: 0; padding: "
			   "20px; "
			   "background-color: #f5f5f5; }"
			   ".container { max-width: 1200px; margin: 0 auto; "
			   "background-color: white; padding: 20px; border-radius: 8px; "
			   "box-shadow: 0 2px 10px rgba(0,0,0,0.1); }"
			   "h1 { color: #333; text-align: center; margin-bottom: 20px; }"
			   ".info-section { background-color: #e9ecef; padding: 15px; "
			   "border-radius: 5px; margin-bottom: 20px; }"
			   ".info-section p { margin: 5px 0; }"
			   "table { width: 100%; border-collapse: collapse; margin: 20px "
			   "0; "
			   "font-size: 14px; }"
			   "th, td { padding: 8px 12px; text-align: left; border: 1px "
			   "solid "
			   "#ddd; }"
			   "th { background-color: #6c757d; color: white; font-weight: "
			   "bold; }"
			   "tr:nth-child(even) { background-color: #f8f9fa; }"
			   "tr:hover { background-color: #e9ecef; }"
			   ".pin-number { font-weight: bold; text-align: center; }"
			   ".status-used { background-color: #28a745; color: white; "
			   "text-align: center; border-radius: 3px; }"
			   ".status-available { background-color: #17a2b8; color: white; "
			   "text-align: center; border-radius: 3px; }"
			   ".status-broken { background-color: #dc3545; color: white; "
			   "text-align: center; border-radius: 3px; }"
			   ".status-not-configured { background-color: #87ceeb; color: "
			   "#333; "
			   "text-align: center; border-radius: 3px; }"
			   ".mode-input { background-color: #ffc107; color: #212529; "
			   "text-align: center; border-radius: 3px; }"
			   ".mode-output { background-color: #28a745; color: white; "
			   "text-align: center; border-radius: 3px; }"
			   ".mode-undefined { background-color: #6c757d; color: white; "
			   "text-align: center; border-radius: 3px; }"
			   ".flag-yes { color: #28a745; font-weight: bold; }"
			   ".flag-no { color: #dc3545; }"
			   ".flag-na { color: #6c757d; font-style: italic; }"
			   ".pin-type { font-family: monospace; font-size: 12px; }"
			   ".legend { margin: 20px 0; padding: 15px; background-color: "
			   "#f8f9fa; border-radius: 5px; }"
			   ".legend h3 { margin-top: 0; }"
			   ".legend-item { display: inline-block; margin: 5px 10px; }"
			   ".legend-color { display: inline-block; width: 20px; height: "
			   "20px; margin-right: 5px; vertical-align: middle; "
			   "border-radius: 3px; }"
			   ".actions { text-align: center; margin: 20px 0; }"
			   ".actions button { padding: 10px 20px; margin: 0 10px; "
			   "background-color: #007bff; color: white; border: none; "
			   "border-radius: 5px; cursor: pointer; font-size: 16px; }"
			   ".actions button:hover { background-color: #0056b3; }"
			   "@media (max-width: 768px) {"
			   "  .container { padding: 10px; }"
			   "  table { font-size: 12px; }"
			   "  th, td { padding: 4px 6px; }"
			   "}"
			   "</style>"
			   "</head>";
	}

	/**
	 * Generates the main pin configuration table
	 * @return String containing HTML table
	 */
	static String generatePinTable() {
		String table = "<table>";
		table += "<thead>";
		table += "<tr>";
		table += "<th>Pin #</th>";
		table += "<th>GPIO</th>";
		table += "<th>Status</th>";
		table += "<th>Mode</th>";
		table += "<th>Pin Type</th>";
		table += "<th>Broken</th>";
#if defined(ESP32) || defined(ESP8266)
		table += "<th>Deep Sleep</th>";
		table += "<th>WiFi Safe</th>";
		table += "<th>Touch GPIO</th>";
#endif
		table += "<th>UI Ref</th>";
		table += "</tr>";
		table += "</thead>";
		table += "<tbody>";

		ESPAllOnPinManager &pinManager = ESPAllOnPinManager::getInstance();

		// Helper function to find pin configuration in PINOUT array
		auto findPinConfig = [](uint8_t gpio) -> ESP_PinMode * {
			for (size_t i = 0;
				 i < static_cast<size_t>(ESP_BoardConf::INITIAL_PINS); i++) {
				if (ESP_BoardConf::PINOUT[i].pin == gpio) {
					return &ESP_BoardConf::PINOUT[i];
				}
			}
			return nullptr;
		};

		// Iterate through all possible GPIO pins (0 to
		// ESP_BoardConf::NUM_PINS-1)
		for (uint8_t gpio = 0; gpio < ESP_BoardConf::NUM_PINS; gpio++) {
			table += "<tr>";

			// Pin number (always sequential)
			table += "<td class='pin-number'>" + String(gpio) + "</td>";

			// GPIO number (same as pin number in this case)
			table += "<td class='pin-number'>" + String(gpio) + "</td>";

			// Find if this GPIO is configured in PINOUT array
			ESP_PinMode *configuredPin = findPinConfig(gpio);

			// Status (Used/Available/Broken/Not Configured)
			bool isAttached = pinManager.isPinAttached(gpio);
			bool isBroken = configuredPin ? configuredPin->isBroken : false;
			String statusClass, statusText;

			if (configuredPin == nullptr) {
				// GPIO not configured in PINOUT array
				statusClass = "status-not-configured";
				statusText = "NOT CONFIGURED";
			} else if (isBroken) {
				statusClass = "status-broken";
				statusText = "BROKEN";
			} else if (isAttached) {
				statusClass = "status-used";
				statusText = "USED";
			} else {
				statusClass = "status-available";
				statusText = "AVAILABLE";
			}

			table += "<td class='" + statusClass + "'>" + statusText + "</td>";

			// Mode (INPUT/OUTPUT/Undefined)
			String modeClass, modeText;

			if (configuredPin) {
				switch (configuredPin->mode) {
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
					break;
				}
			} else {
				modeClass = "mode-undefined";
				modeText = "NOT CONFIGURED";
			}

			table += "<td class='" + modeClass + "'>" + modeText + "</td>";

			// Pin Type
			String pinTypeName;
			if (configuredPin) {
				pinTypeName = getPinTypeName(configuredPin->type);
			} else {
				pinTypeName = "None";
			}
			table += "<td class='pin-type'>" + String(pinTypeName) + "</td>";

			// Broken status
			String brokenText = isBroken ? "YES" : "NO";
			String brokenClass = isBroken ? "flag-yes" : "flag-no";
			table += "<td class='" + brokenClass + "'>" + brokenText + "</td>";

#if defined(ESP32) || defined(ESP8266)
			// ESP-specific flags
			String deepSleepText, deepSleepClass;
			String wifiText, wifiClass;
			String touchText, touchClass;

			if (configuredPin) {
				deepSleepText = configuredPin->canDeepSleep ? "YES" : "NO";
				deepSleepClass =
					configuredPin->canDeepSleep ? "flag-yes" : "flag-no";

				wifiText = configuredPin->canUseWithWiFi ? "YES" : "NO";
				wifiClass =
					configuredPin->canUseWithWiFi ? "flag-yes" : "flag-no";

				touchText = configuredPin->isTouchGPIO ? "YES" : "NO";
				touchClass =
					configuredPin->isTouchGPIO ? "flag-yes" : "flag-no";
			} else {
				deepSleepText = wifiText = touchText = "N/A";
				deepSleepClass = wifiClass = touchClass = "flag-na";
			}

			table +=
				"<td class='" + deepSleepClass + "'>" + deepSleepText + "</td>";
			table += "<td class='" + wifiClass + "'>" + wifiText + "</td>";
			table += "<td class='" + touchClass + "'>" + touchText + "</td>";
#endif

			// UI Reference
			auto &pinMap = pinManager.getPINMap();
			uint16_t uiRef = pinMap[gpio];
			String uiRefText = (uiRef == 0) ? "-" : String(uiRef);
			if (uiRef == 10000) {
				uiRefText = "BLOCKED";
			}
			table += "<td>" + uiRefText + "</td>";

			table += "</tr>";
		}

		table += "</tbody>";
		table += "</table>";
		return table;
	}

	/**
	 * Generates the color legend for the table
	 * @return String containing HTML legend
	 */
	static String getTableLegend() {
		return "<div class='legend'>"
			   "<h3>Legend</h3>"
			   "<div class='legend-item'>"
			   "<span class='legend-color' style='background-color: "
			   "#28a745;'></span>Used/Output/Yes"
			   "</div>"
			   "<div class='legend-item'>"
			   "<span class='legend-color' style='background-color: "
			   "#17a2b8;'></span>Available"
			   "</div>"
			   "<div class='legend-item'>"
			   "<span class='legend-color' style='background-color: "
			   "#87ceeb;'></span>Not Configured"
			   "</div>"
			   "<div class='legend-item'>"
			   "<span class='legend-color' style='background-color: "
			   "#dc3545;'></span>Broken/No"
			   "</div>"
			   "<div class='legend-item'>"
			   "<span class='legend-color' style='background-color: "
			   "#ffc107;'></span>Input"
			   "</div>"
			   "<div class='legend-item'>"
			   "<span class='legend-color' style='background-color: "
			   "#6c757d;'></span>Undefined"
			   "</div>"
			   "</div>";
	}

	/**
	 * Gets current timestamp for display
	 * @return String containing formatted timestamp
	 */
	static String getTimestamp() {
		unsigned long currentTime = millis();
		unsigned long seconds = currentTime / 1000;
		unsigned long minutes = seconds / 60;
		unsigned long hours = minutes / 60;

		return "Uptime: " + String(hours % 24) + "h " + String(minutes % 60) +
			   "m " + String(seconds % 60) + "s";
	}
};

#endif
