#ifndef _STEPPER_RUNNER_H
#define _STEPPER_RUNNER_H

#include <memory>
#include <vector>

#include <Arduino.h>

/**
 * Interface for objects that need to run continuously in the main loop
 */
class IRunnable {
  public:
	virtual ~IRunnable() = default;
	virtual void run() = 0;
	virtual bool isActive() const = 0;
	virtual String getID() const = 0;
	virtual String getDriverName() const = 0;
};

/**
 * Singleton class to manage multiple AccelStepper instances
 * and execute them in a non-blocking manner in the main loop
 */
class StepperRunner {
  private:
	static constexpr size_t MAX_STEPPERS = 16; // Maximum number of steppers
	std::vector<std::shared_ptr<IRunnable>> _runnables;
	static StepperRunner *_instance;

	StepperRunner() { _runnables.reserve(MAX_STEPPERS); }

  public:
	/**
	 * Get singleton instance
	 */
	static StepperRunner &getInstance() {
		if (_instance == nullptr) {
			_instance = new StepperRunner();
		}
		return *_instance;
	}

	/**
	 * Register a runnable object (stepper, motor controller, etc.)
	 * @param runnable Shared pointer to the runnable object
	 * @return True if registered successfully, false if array is full
	 */
	bool registerRunnable(std::shared_ptr<IRunnable> runnable) {
		if (_runnables.size() < MAX_STEPPERS) {
			_runnables.push_back(runnable);
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
		auto it = std::remove_if(_runnables.begin(), _runnables.end(),
								 [&id](const std::shared_ptr<IRunnable> &r) {
									 return r->getID() == id;
								 });
		if (it != _runnables.end()) {
			_runnables.erase(it, _runnables.end());
			return true;
		}
		return false;
	}

	/**
	 * Execute all registered runnable objects
	 * This method should be called in the main loop
	 */
	void runAll() {
		for (auto &runnable : _runnables) {
			if (runnable && runnable->isActive()) {
				runnable->run();
			}
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
};

// Static member definition
StepperRunner *StepperRunner::_instance = nullptr;
#endif