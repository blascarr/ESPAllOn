#include <Arduino.h>
#include <ESPUI.h>

void addElementWithParent(std::map<uint16_t, uint16_t> &map, uint16_t elementId,
						  uint16_t parentId) {
	map[elementId] = parentId;
}
