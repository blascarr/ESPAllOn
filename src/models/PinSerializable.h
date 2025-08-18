#ifndef _PINSERIALIZABLE_H
#define _PINSERIALIZABLE_H
#include <Arduino.h>
#include <ArduinoJson.h>

#include "../manager/Storage_Manager.h"
#include <PinManager.h>

/**
 * Serializable pin configuration class
 * Extends PinMode with JSON serialization capabilities for persistent storage
 */
class pinSerializable : public PinMode, public IStringSerializable {
  public:
	/**
	 * Constructor for output pin configuration
	 */
	pinSerializable(uint8_t pin, OutputPin isOutput, PinType type)
		: PinMode(pin, isOutput, type) {}

	/**
	 * Constructor for input pin configuration
	 */
	pinSerializable(uint8_t pin, InputPin isInput, PinType type)
		: PinMode(pin, isInput, type) {}
	/**
	 * Serialize pin configuration to JSON string
	 * @return JSON string representation of the pin configuration
	 */
	String serialize() override {
		StaticJsonDocument<256> doc;

		doc["pin"] = pin;
		doc["isBroken"] = isBroken;

		String output;
		serializeJson(doc, output);
		return output;
	}

	/**
	 * Deserialize pin configuration from JSON string
	 * @param data JSON string containing pin configuration
	 * @return True if deserialization was successful
	 */
	bool deserialize(const String &data) override {
		StaticJsonDocument<256> doc;
		DeserializationError error = deserializeJson(doc, data);
		if (error) {
			return false;
		}
		isBroken = doc["isBroken"];
		return true;
	}
};
#endif