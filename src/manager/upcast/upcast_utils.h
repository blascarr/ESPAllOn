#ifndef _ESPINNER_UPCAST_UTILS_H
#define _ESPINNER_UPCAST_UTILS_H

#include "../../controllers/ESPinner.h"

// Compatibility layer for C++11 - provides make_unique if not available
#if __cplusplus < 201402L
#include <memory>
#include <utility> // For std::forward

namespace std {
/**
 * make_unique implementation for C++11 compatibility
 * Creates a unique_ptr with the given arguments
 */
template <typename T, typename... Args>
unique_ptr<T> make_unique(Args &&...args) {
	return unique_ptr<T>(new T(forward<Args>(args)...));
}
} // namespace std

#endif

/**
 * Factory method implementation for creating ESPinner instances
 * Creates specific ESPinner types based on string identifier
 * @param type String identifier for the ESPinner type
 * @return Unique pointer to the created ESPinner instance
 */
std::unique_ptr<ESPinner> ESPinner::create(const String &type) {
	if (type == "ESPINNER_GPIO") {
		auto ptr = std::make_unique<ESPinner_GPIO>();
		return std::unique_ptr<ESPinner>(
			static_cast<ESPinner *>(ptr.release()));
	}
	if (type == "ESPINNER_ENCODER") {
		auto ptr = std::make_unique<ESPinner_Encoder>();
		return std::unique_ptr<ESPinner>(
			static_cast<ESPinner *>(ptr.release()));
	}
	if (type == "ESPINNER_DC") {
		auto ptr = std::make_unique<ESPinner_DC>();
		return std::unique_ptr<ESPinner>(
			static_cast<ESPinner *>(ptr.release()));
	}
	if (type == "ESPINNER_MPU") {
		auto ptr = std::make_unique<ESPinner_MPU>();
		return std::unique_ptr<ESPinner>(
			static_cast<ESPinner *>(ptr.release()));
	}
	if (type == "ESPINNER_NEOPIXEL") {
		auto ptr = std::make_unique<ESPinner_Neopixel>();
		return std::unique_ptr<ESPinner>(
			static_cast<ESPinner *>(ptr.release()));
	}
	if (type == "ESPINNER_STEPPER") {
		auto ptr = std::make_unique<ESPinner_Stepper>();
		return std::unique_ptr<ESPinner>(
			static_cast<ESPinner *>(ptr.release()));
	}
	if (type == "ESPINNER_RFID") {
		auto ptr = std::make_unique<ESPinner_RFID>();
		return std::unique_ptr<ESPinner>(
			static_cast<ESPinner *>(ptr.release()));
	}
	if (type == "ESPINNER_LCD") {
		auto ptr = std::make_unique<ESPinner_LCD>();
		return std::unique_ptr<ESPinner>(
			static_cast<ESPinner *>(ptr.release()));
	}
	return nullptr; // Return nullptr if type not found
}

#endif