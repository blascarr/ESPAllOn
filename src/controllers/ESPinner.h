#ifndef _ESPINNER_H
#define _ESPINNER_H
#define NVS

#include <ESPUI.h>
#include <Persistance.h>

/**
 * Enumeration of available ESPinner module types
 */
enum class ESPinner_Mod {
	GPIO,	  // Basic GPIO input/output
	Stepper,  // Stepper motor control
	RFID,	  // RFID reader module
	NeoPixel, // Addressable LED strips
	VOID,	  // Empty/undefined module
	DC,		  // DC motor control
	MPU,	  // Motion processing unit (accelerometer/gyroscope)
	Encoder,  // Rotary encoder input
	TFT,	  // TFT display module
	LCD		  // LCD display module
};
struct ESPinner_Module {
	ESPinner_Mod model; // Module type enumeration
	String name;		// Human-readable module name
};

const ESPinner_Module mods[] = {{ESPinner_Mod::VOID, VOID_LABEL},
								{ESPinner_Mod::GPIO, GPIO_LABEL},
								{ESPinner_Mod::Stepper, STEPPER_LABEL},
								{ESPinner_Mod::RFID, RFID_LABEL},
								{ESPinner_Mod::NeoPixel, NEOPIXEL_LABEL},
								{ESPinner_Mod::DC, DC_LABEL},
								{ESPinner_Mod::MPU, MPU_LABEL},
								{ESPinner_Mod::Encoder, ENCODER_LABEL},
								{ESPinner_Mod::TFT, TFT_LABEL},
								{ESPinner_Mod::LCD, LCD_LABEL}};

/**
 * Abstract interface for all ESPinner modules
 * Defines the common interface that all ESPinner implementations
 */
class IESPinner {
  public:
	IESPinner() {}

	/**
	 * Initialize the ESPinner module
	 * Called once during system startup to configure hardware
	 */
	virtual void setup() = 0;

	/**
	 * Called periodically to handle ongoing operations
	 */
	virtual void update() = 0;

	/**
	 * Load the module's interface
	 * Called to set up the user interface elements
	 */
	virtual void loader() = 0;

	/**
	 * Implement the module functionality
	 * Called to activate the module and integrate it with the system
	 */
	virtual void implement() = 0;

	virtual ~IESPinner() {}
};

/**
 * Base implementation of ESPinner with JSON serialization support
 * It implements the IESPinner interface and adds JSON serialization
 * capabilities for persistent storage. Each ESPinner has a unique ID and module
 * type.
 */
class ESPinner : public IESPinner, public IJSONSerializable {
  public:
	ESPinner_Mod mod; // Module type of this ESPinner
	String ID;		  // Unique identifier for this ESPinner instance

	/**
	 * Factory method to create ESPinner instances based on type string
	 * @param type String representation of the ESPinner type
	 * @return Unique pointer to the created ESPinner instance
	 */
	static std::unique_ptr<ESPinner> create(const String &type);

	/**
	 * Constructor with specific module type
	 * @param espinner_mod Type of ESPinner module to create
	 */
	ESPinner(ESPinner_Mod espinner_mod) : mod(espinner_mod) {};

	/**
	 * Constructor - creates VOID type ESPinner
	 */
	ESPinner() : mod(ESPinner_Mod::VOID) {}

	void setup() override {}
	void update() override {}
	void implement() override {}
	void loader() override {}

	/**
	 * Sets the module type
	 * @param espinner_mod New module type
	 */
	void setType(ESPinner_Mod espinner_mod) { mod = espinner_mod; }

	/**
	 * Gets the module type
	 * @return Current module type
	 */
	ESPinner_Mod getType() { return mod; }

	/**
	 * Sets the unique identifier
	 * @param espinner_ID New unique identifier
	 */
	void setID(String espinner_ID) { ID = espinner_ID; }

	/**
	 * Gets the unique identifier
	 * @return Current unique identifier
	 */
	String getID() { return ID; }
};

#include "mods/ESPinner_DC/ESPinner_DC.h"
#include "mods/ESPinner_Encoder/ESPinner_Encoder.h"
#include "mods/ESPinner_GPIO/ESPinner_GPIO.h"
#include "mods/ESPinner_LCD/ESPinner_LCD.h"
#include "mods/ESPinner_MPU/ESPinner_MPU.h"
#include "mods/ESPinner_NeoPixel/ESPinner_NeoPixel.h"
#include "mods/ESPinner_RFID/ESPinner_RFID.h"
#include "mods/ESPinner_Stepper/ESPinner_Stepper.h"
#include "mods/ESPinner_TFT/ESPinner_TFT.h"
#endif