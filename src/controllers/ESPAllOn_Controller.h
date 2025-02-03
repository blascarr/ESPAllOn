#ifndef _ESPALLON_CONTROLLER_H
#define _ESPALLON_CONTROLLER_H

#include <TickerFree.h>

using StateChangeCallback = void (*)(status_t previousState,
									 status_t currentState);

class ESPALLON_Controller : public TickerFree<> {
  public:
	status_t currentState;
	status_t previousState;
	virtual void begin() {}
	ESPALLON_Controller() : TickerFree<>([this]() { this->tick(); }, 10) {}

	virtual void connectTick() {}
	void tick() {};
};

#endif