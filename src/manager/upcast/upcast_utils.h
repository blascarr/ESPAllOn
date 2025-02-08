#ifndef _ESPINNER_UPCAST_UTILS_H
#define _ESPINNER_UPCAST_UTILS_H

#include "../../controllers/ESPinner.h"

#if __cplusplus < 201402L
#include <memory>
#include <utility> // For std::forward

namespace std {
template <typename T, typename... Args>
unique_ptr<T> make_unique(Args &&...args) {
	return unique_ptr<T>(new T(forward<Args>(args)...));
}
} // namespace std

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
		auto ptr = std::make_unique<ESPinner_NEOPIXEL>();
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
	return nullptr;
}

#endif

#endif