#include <Arduino.h>
#include <ArduinoJson.h>
#include <Persistance.h>
#include <PinManager.h>

class pinSerializable : public PinMode, public IStringSerializable {
  public:
	pinSerializable(uint8_t pin, OutputPin isOutput, PinType type)
		: PinMode(pin, isOutput, type) {}
	pinSerializable(uint8_t pin, InputPin isInput, PinType type)
		: PinMode(pin, isInput, type) {}
	String serialize() override {
		StaticJsonDocument<256> doc;

		doc["pin"] = pin;
		doc["isBroken"] = isBroken;

		String output;
		serializeJson(doc, output);
		return output;
	}

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