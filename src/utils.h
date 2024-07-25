#include <Arduino.h>
#include <ESPUI.h>

// ------------------ SERIAL CONFIG --------------------//
#define PRINTDEBUG true
#define SERIALDEBUG Serial

#define DUMPS(s)                                                               \
	{ SERIALDEBUG.print(F(s)); }
#define DUMPSLN(s)                                                             \
	{ SERIALDEBUG.println(F(s)); }
#define DUMPPRINTLN()                                                          \
	{ SERIALDEBUG.println(); }
#define DUMPV(v)                                                               \
	{ SERIALDEBUG.print(v); }
#define DUMPLN(s, v)                                                           \
	{                                                                          \
		DUMPS(s);                                                              \
		DUMPV(v);                                                              \
		DUMPPRINTLN();                                                         \
	}
#define DUMP(s, v)                                                             \
	{                                                                          \
		DUMPS(s);                                                              \
		DUMPV(v);                                                              \
	}

void addElementWithParent(std::map<uint16_t, uint16_t> &map, uint16_t elementId,
						  uint16_t parentId) {
	map[elementId] = parentId;
}

void debugMap(std::map<uint16_t, uint16_t> &map) {
	for (auto it = map.begin(); it != map.end(); ++it) {
		DUMP(" Key: ", it->first);
		DUMP(" Value: ", it->second);
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
std::map<uint8_t, const char *> gpioLabels = {
	{0, "0"},	{1, "1"},	{2, "2"},	{3, "3"},	{4, "4"},	{5, "5"},
	{6, "6"},	{7, "7"},	{8, "8"},	{9, "9"},	{10, "10"}, {11, "11"},
	{12, "12"}, {13, "13"}, {14, "14"}, {15, "15"}, {16, "16"}, {17, "17"},
	{18, "18"}, {19, "19"}, {20, "20"}, {21, "21"}, {22, "22"}, {23, "23"},
	{24, "24"}, {25, "25"}, {26, "26"}, {27, "27"}, {28, "28"}, {29, "29"},
	{30, "30"}, {31, "31"}, {32, "32"}, {33, "33"}, {34, "34"}, {35, "35"},
	{36, "36"}, {37, "37"}, {38, "38"}, {39, "39"}};

const char *getLabelForPin(uint8_t pin) {
	if (gpioLabels.find(pin) != gpioLabels.end()) {
		return gpioLabels[pin];
	} else {
		return "Pin not found";
	}
}