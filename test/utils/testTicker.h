/**
 * Test Ticker Utility
 *
 * This utility provides ticker-based test execution functionality for managing
 * sequential test execution with time delays.
 *
 * Usage:
 * 1. Add test functions to testFunctions vector
 * 2. Create TickerFree instance with runTest as callback
 * 3. Start ticker to begin sequential test execution
 * 4. Tests execute automatically with specified intervals
 */

#ifndef _UTILS_TEST_H
#define _UTILS_TEST_H

#include "../config.h"
#include "TickerFree.h"
#include <unity.h>
#include <vector>

/** Type definition for test function pointers */
typedef std::function<void()> TestFunction;

/** Global vector storing test functions for sequential execution */
std::vector<TestFunction> testFunctions;

/** Current test index for sequential execution tracking */
size_t currentTestIndex = 0;

/**
 * Main test runner function for ticker-based execution
 *
 * Executes test functions sequentially from the testFunctions vector.
 * Automatically ends Unity test framework when all tests complete.
 * Called periodically by TickerFree for time-based test execution.
 */
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