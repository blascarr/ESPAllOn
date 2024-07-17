#include <Arduino.h>
#include <ESPUI.h>

void addElementWithParent(std::map<uint16_t, uint16_t> &map, uint16_t elementId,
						  uint16_t parentId) {
	map[elementId] = parentId;
}

void debugMap(std::map<uint16_t, uint16_t> &map) {
	for (auto it = map.begin(); it != map.end(); ++it) {
		Serial.print("Clave: ");
		Serial.print(it->first);
		Serial.print(" Value: ");
		Serial.print(it->second);
	}
}

uint16_t getParentId(const std::map<uint16_t, uint16_t> &map,
					 uint16_t elementId) {
	auto it = map.find(elementId);
	if (it != map.end()) {
		return it->second;
	} else {
		return 0;
	}
}

void removeControlId(std::vector<uint16_t> &controlRef, uint16_t id) {
	auto newEnd = std::remove(controlRef.begin(), controlRef.end(), id);
	controlRef.erase(newEnd, controlRef.end());
}

void removeValueFromMap(std::map<uint16_t, uint16_t> &elementToParentMap,
						uint16_t value) {
	std::vector<uint16_t> keysToRemove;

	for (const auto &pair : elementToParentMap) {
		if (pair.second == value) {
			keysToRemove.push_back(pair.first);
		}
	}

	for (uint16_t key : keysToRemove) {
		elementToParentMap.erase(key);
	}
}
