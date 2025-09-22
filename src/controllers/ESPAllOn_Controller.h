#ifndef _ESPALLON_CONTROLLER_H
#define _ESPALLON_CONTROLLER_H

#include <TickerFree.h>

/** @brief Callback function type for state change events */
using StateChangeCallback = void (*)(status_t previousState,
									 status_t currentState);

/**
 *
 * ESPALLON_Controller provides a foundation for all controllers in the system,
 * inheriting from TickerFree to provide periodic execution capabilities and
 * state management functionality.
 */
class ESPALLON_Controller : public TickerFree<> {
  public:
	status_t currentState;	///< Current state of the controller
	status_t previousState; ///< Previous state of the controller

	/**
	 * Virtual function for controller initialization
	 */
	virtual void begin() {}

	/**
	 * Constructor initializes the ticker with default interval
	 * Sets up a 100ms ticker interval and binds the tick() method
	 */
	ESPALLON_Controller() : TickerFree<>([this]() { this->tick(); }, 100) {}

	/**
	 * Virtual function for connection tick handling
	 */
	virtual void connectTick() {}

	/**
	 * Tick function called periodically by the ticker
	 */
	void tick() {};
};

#endif