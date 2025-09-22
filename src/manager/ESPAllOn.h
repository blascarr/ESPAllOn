#ifndef _ESPALLON_H
#define _ESPALLON_H

#include "../controllers/ESPAllOn_Wifi.h"

#include "../controllers/ESPinner.h"
#include "../controllers/UI/TabController.h"
#include "../controllers/Wifi_Controller.h"
#include "../utils.h"

#include "../controllers/ESPAction.h"

#include <TickerFree.h>

/*----------------------------------------------------*/
/*----------------------------------------------------*/
/*----------------------------------------------------*/

/**
 * Generic text callback for UI controls
 *
 * This callback is used to handle text input changes in UI controls.
 * It doesn't perform any specific action but is required by ESPUI.
 */
void textCallback(Control *sender, int type) {}

/**
 * Forward declaration of ESPinner selector function
 */
void ESPinnerSelector();

/**
 * Callback function to remove all pin configurations
 *
 * This function clears all stored pin configurations when the
 * remove button is pressed in the UI.
 */
void removeConfig(Control *sender, int type) {
	if (type == B_UP) {
		DUMPSLN("REMOVE PIN CONFIG");
		ESPinner_Manager::getInstance().clearPinConfigInStorage();
	}
}

/*----------------------------------------------------*/
/*----------------------------------------------------*/
/*----------------------------------------------------*/

/**
 * Main application manager class for ESPAllOn system
 *
 * ESPAllOn serves as the central coordinator for the entire system,
 * implementing the Singleton pattern. It manages system initialization,
 * UI setup, WiFi configuration, action management, and coordinates
 * between different subsystems.
 */

class ESPAllOn {

  public:
	/**
	 * Constructor initializes capacity and size
	 */
	ESPAllOn() : capacity(MOD_CAPACITY), size(0) {}

	/**
	 * Gets the singleton instance of ESPAllOn
	 * @return Reference to the singleton instance
	 */
	static ESPAllOn &getInstance() {
		static ESPAllOn instance;
		return instance;
	}
	/**
	 * Setup function initializes the main UI components
	 *
	 * Creates the initial ESPinner selector interface. For ESP8266,
	 * uses IRAM heap allocation to optimize memory usage.
	 */
	void setup() {

#ifdef ESP8266
		{
			HeapSelectIram doAllocationsInIRAM;
#endif
			// Creates first ESPinner Controller
			ESPinnerSelector();

#ifdef ESP8266
		} // HeapSelectIram
#endif
	}
	/**
	 * Creates the WiFi configuration tab in the UI
	 *
	 * Sets up UI controls for WiFi SSID/password input, save functionality,
	 * and pin configuration removal. Includes input validation and
	 * maximum length constraints.
	 */
	void wifiTab() {
		auto wifitab = getTab(TabType::NetworkTab);
		ESPALLON_Wifi::getInstance().wifi_ssid_text =
			ESPUI.addControl(ControlType::Text, SSID_LABEL, VOID_VALUE,
							 ControlColor::Alizarin, wifitab, textCallback);
		ESPUI.addControl(ControlType::Max, VOID_VALUE, "32", None,
						 ESPALLON_Wifi::getInstance().wifi_ssid_text);
		ESPALLON_Wifi::getInstance().wifi_pass_text =
			ESPUI.addControl(ControlType::Password, PASS_LABEL, VOID_VALUE,
							 ControlColor::Alizarin, wifitab, textCallback);
		ESPUI.addControl(ControlType::Max, VOID_VALUE, "64", None,
						 ESPALLON_Wifi::getInstance().wifi_pass_text);

		// Register controls in the WiFi UI map for easy access
		extern std::map<String, uint16_t> WIFI_UI_ref;
		WIFI_UI_ref[SSID_LABEL] = ESPALLON_Wifi::getInstance().wifi_ssid_text;
		WIFI_UI_ref[PASS_LABEL] = ESPALLON_Wifi::getInstance().wifi_pass_text;

		ESPUI.addControl(ControlType::Button, SAVE_LABEL, SAVE_VALUE,
						 ControlColor::Peterriver, wifitab,
						 enterWifiDetailsCallback);

		ESPUI.addControl(ControlType::Button, REMOVE_PINCONFIG_LABEL,
						 REMOVE_PINCONFIG_VALUE, ControlColor::Peterriver,
						 wifitab, removeConfig);
	}

	/**
	 * Creates the action linking tab in the UI
	 *
	 * Sets up the interface for linking actions to configured devices.
	 * Creates a selector with all available actions and a button to
	 * perform the linking operation.
	 */
	void ESPActionSelector() {
		auto LinkActionTab = getTab(TabType::LinkedActions);

		uint16_t actionSelector = ESPUI.addControl(
			ControlType::Select, ACTION_LABEL, VOID_VALUE,
			ControlColor::Wetasphalt, LinkActionTab, generalCallback);

		for (const ESPAction &action : actions) {
			ESPUI.addControl(ControlType::Option, action.getName().c_str(),
							 action.getReference(), None, actionSelector);
		}
		for (const ESPAction &action : actions) {
			ESPUI.addControl(ControlType::Option, action.getName().c_str(),
							 action.getReference(), None, actionSelector);
		}

		uint16_t GPIO_Link_Action = ESPUI.addControl(
			ControlType::Button, LINKACTION_LABEL, LINKACTION_VALUE,
			ControlColor::Alizarin, actionSelector, voidCallback);
	}

	/**
	 * Begins the ESPUI web server and initializes all tabs
	 *
	 * Sets up the link items tab, WiFi configuration tab, and starts
	 * the ESPUI web server with the configured hostname.
	 */
	void begin() {
		linkItemsTab();

		wifiTab();
		ESPUI.begin(HOSTNAME);
	}

	/**
	 * Save function for system configuration
	 * @todo Implement configuration saving functionality
	 */
	void save() {

	};

	/**
	 * Sets up the linked items tab
	 */
	void linkItemsTab() { ESPActionSelector(); }

	/**
	 * Debug function for system diagnostics
	 * @todo Implement debug functionality
	 */
	void debug() {

	};

	/**
	 * Adds a device to the system
	 * @param device Pointer to the ESPinner device to add
	 * @todo Implement device addition functionality
	 */
	void addDevice(ESPinner *device);

	/**
	 * Adds an action to the available actions list
	 * @param action ESPAction to add to the system
	 */
	void addAction(const ESPAction &action) { actions.push_back(action); }

	/**
	 * Assigns an action to a specific pin
	 * @param pin Pin number to assign the action to
	 * @param actionName Name of the action to assign
	 * @todo Complete the implementation of action assignment
	 */
	void assignActionToPin(uint16_t pin, const String &actionName) {
		for (const ESPAction &action : actions) {
			// if (action.getName() == actionName) {
			// pinActions[pin] = action;
			// break;
			// }
		}
	}

	/**
	 * Triggers an action associated with a pin
	 * @param pin Pin number whose action should be triggered
	 */
	void triggerPin(uint16_t pin) {
		auto it = pinActions.find(pin);
		if (it != pinActions.end()) {
			it->second.execute(12);
		}
	}

  private:
	ESPAllOnPinManager *pinManager; // Pointer to the pin manager instance
	int capacity;					// Maximum capacity for modules
	int size;						// Current number of modules
	std::vector<ESPAction> actions; // List of available actions
	std::map<uint16_t, ESPAction>
		pinActions; // Map of pin-to-action assignments
};

#endif