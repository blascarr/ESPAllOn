#ifndef _NEOPIXEL_RUNNER_H
#define _NEOPIXEL_RUNNER_H

#include <Adafruit_NeoPixel.h>
#include <memory>
#include <vector>

enum NEOPIXEL_ANIMATION {
	VOID = 0,
	BLINK = 1,
	FADE = 2,
	INCREMENTAL = 3,
	DECREMENTAL = 4,
	RAINBOW = 5
};

typedef struct {
	uint8_t val[3]; // the evaluated colour data (RGB value 0-255 or other)
} RGBColor;

enum NEOPIXEL_COLOR {
	BLACK = 0,
	WHITE = 1,
	RED = 2,
	GREEN = 3,
	BLUE = 4,
	YELLOW = 5,
	CYAN = 6,
	MAGENTA = 7,
	ORANGE = 8,
	PURPLE = 9
};
/**
 * Interface for NeoPixel objects that need to run continuously
 */
class INeopixelRunnable {
  public:
	String ID;
	virtual ~INeopixelRunnable() = default;
	virtual void run() = 0;
	virtual bool isActive() const = 0;
	String get_ID() { return ID; }
};

/**
 * Singleton class to manage multiple NeoPixel instances
 * and execute them using Ticker for non-blocking operation
 */
class NeopixelRunner {
  private:
	static constexpr size_t MAX_NEOPIXELS = 8; // Maximum number of NeoPixels
	std::vector<std::shared_ptr<INeopixelRunnable>> _runnables;
	static NeopixelRunner *_instance;
	bool _isRunning = false;

	NeopixelRunner() { _runnables.reserve(MAX_NEOPIXELS); }

  public:
	/**
	 * Get singleton instance
	 */
	static NeopixelRunner &getInstance() {
		if (_instance == nullptr) {
			_instance = new NeopixelRunner();
		}
		return *_instance;
	}

	/**
	 * Register a NeoPixel runnable object
	 * @param runnable Shared pointer to the runnable object
	 * @return True if registered successfully, false if array is full
	 */
	bool registerRunnable(std::shared_ptr<INeopixelRunnable> runnable) {
		if (_runnables.size() < MAX_NEOPIXELS) {
			_runnables.push_back(runnable);
			if (!_isRunning) {
				// startTicker();
			}
			return true;
		}
		return false;
	}

	/**
	 * Unregister a runnable object by ID
	 * @param id ID of the object to unregister
	 * @return True if found and removed, false otherwise
	 */
	bool unregisterRunnable(const String &id) {
		auto it =
			std::remove_if(_runnables.begin(), _runnables.end(),
						   [&id](const std::shared_ptr<INeopixelRunnable> &r) {
							   return r->get_ID() == id;
						   });
		if (it != _runnables.end()) {
			_runnables.erase(it, _runnables.end());
			if (_runnables.empty()) {
				// stopTicker();
			}
			return true;
		}
		return false;
	}

	/**
	 * Update all active NeoPixel runnables
	 */
	void runAll() {
		for (auto &runnable : _runnables) {
			if (runnable && runnable->isActive()) {
				runnable->run();
			}
		}
	}

	/**
	 * Static callback for Ticker
	 */
	static void tickerCallback() {
		if (_instance) {
			_instance->runAll();
		}
	}

	/**
	 * Get number of registered runnables
	 */
	size_t getCount() const { return _runnables.size(); }

	/**
	 * Clear all registered runnables
	 */
	void clear() { _runnables.clear(); }

	/**
	 * Get runnable by ID
	 */
	std::shared_ptr<INeopixelRunnable> getRunnable(const String &id) {
		for (auto &runnable : _runnables) {
			if (runnable && runnable->get_ID() == id) {
				return runnable;
			}
		}
		return nullptr;
	}
};

// Static member definition
NeopixelRunner *NeopixelRunner::_instance = nullptr;

#endif
