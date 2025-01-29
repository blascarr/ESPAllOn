#ifndef _UTILS_TEST_H
#define _UTILS_TEST_H

#include <Arduino.h>
#include <Wire.h>
#include <unity.h>

#include "../config.h"
#include "TickerFree.h"
#include <vector>

typedef std::function<void()> TestFunction;
std::vector<TestFunction> testFunctions;
size_t currentTestIndex = 0;

void runTest() {
	if (currentTestIndex < testFunctions.size()) {
		testFunctions[currentTestIndex](); // Execute Current Test
		currentTestIndex++;				   // Next Index
	} else {
		DUMPSLN("All tests completed");
		UNITY_END();
	}
}
#endif