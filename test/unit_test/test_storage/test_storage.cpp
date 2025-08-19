/**
 * Storage Unit Test
 *
 * This test validates the persistent storage functionality for ESPinner objects
 * using the Non-Volatile Storage (NVS) system. It tests storage, retrieval,
 * and management of ESPinner configurations in memory and persistent storage.
 *
 * Test Steps:
 * 1. Create multiple ESPinner objects (GPIO and DC types)
 * 2. Push ESPinners to the manager and validate list size
 * 3. Test storage persistence by checking NVS for saved data
 * 4. Load ESPinners from storage and validate restoration
 * 5. Clear storage and verify data removal
 * 6. Validate empty storage state after cleanup
 */

#include "../../config.h"

#include "../../../src/manager/ESPinner_Manager.h"

#include "../../utils/persistance_utils.h"
#include "../../utils/testTicker.h"

std::list<std::unique_ptr<ESPinner>> ESPinnerTestList;

/**
 * Test that ESPinner data exists in NVS storage
 * Validates that previously saved ESPinner configurations are present
 */
void test_is_there_espinner_in_file() {
	bool isThereAnyESPinner = isThereAnyInfoInNVS(ESPinner_Path);
	TEST_ASSERT_TRUE(isThereAnyESPinner);
}

/**
 * Test that no ESPinner data exists in NVS storage
 * Validates that storage is empty after cleanup operations
 */
void test_not_any_espinner_in_file() {
	bool isThereAnyESPinner = isThereAnyInfoInNVS(ESPinner_Path);
	TEST_ASSERT_FALSE(isThereAnyESPinner);
}

/**
 * Test ESPinner manager size validation
 * Confirms that exactly 3 ESPinners are loaded in the manager
 */
void test_espinners_size() {
	size_t ESPinnersLength = ESPinner_Manager::getInstance().espinnerSize();
	// size_t es un alias de unsigned int
	TEST_ASSERT_EQUAL_UINT32(ESPinnersLength, 3);
}

void setup() {
	Serial.begin(115200);
	UNITY_BEGIN();

	auto espinnerGPIO_1 = std::make_unique<ESPinner_GPIO>();
	espinnerGPIO_1->setGPIO(5);
	espinnerGPIO_1->setGPIOMode(GPIOMode::Input);
	espinnerGPIO_1->setID("ESPINNER GPIO 1");

	auto espinnerGPIO_2 = std::make_unique<ESPinner_GPIO>();
	espinnerGPIO_2->setGPIO(10);
	espinnerGPIO_2->setGPIOMode(GPIOMode::Output);
	espinnerGPIO_2->setID("ESPINNER GPIO 2");

	auto espinnerDC = std::make_unique<ESPinner_DC>();
	espinnerDC->setGPIOA(11);
	espinnerDC->setGPIOB(12);
	espinnerDC->setID("ESPINNER DC");

	// ESPinnerTestList.push_back(std::move(espinnerGPIO_1));
	// ESPinnerTestList.push_back(std::move(espinnerGPIO_2));
	// ESPinnerTestList.push_back(std::move(espinnerDC));

	// Push Espinners
	ESPinner_Manager::getInstance().push(std::move(espinnerGPIO_1));
	ESPinner_Manager::getInstance().push(std::move(espinnerGPIO_2));
	ESPinner_Manager::getInstance().push(std::move(espinnerDC));

	/*
	std::for_each(ESPinners.begin(), ESPinners.end(), [](const
std::unique_ptr<ESPinner>& espinner) {
});
	for (auto it = ESPinnerTestList.begin(); it != ESPinnerTestList.end();
		 ++it) {
		// ESPinner_Manager::getInstance().push(std::move(it));
		(*it)->update();
	}
		*/

	// Length of Espinners in List
	RUN_TEST(test_espinners_size);

	RUN_TEST(test_is_there_espinner_in_file);

	ESPinner_Manager::getInstance().loadFromStorage();
	// Length of Espinners in List after Load
	RUN_TEST(test_espinners_size);

	ESPinner_Manager::getInstance().clearPinConfigInStorage();
	RUN_TEST(test_espinners_size);

	ESPinner_Manager::getInstance().loadFromStorage();
	RUN_TEST(test_not_any_espinner_in_file);
	UNITY_END();
}

void loop() {}
