#ifndef _MOTION_CONTROLLER_H
#define _MOTION_CONTROLLER_H

#include "./IStepperDriver.h"
#include <AccelStepper.h>

class MotionController {
  public:
	MotionController(AccelStepper &stepper, IStepperDriver &driver)
		: st(stepper), drv(driver) {}

	void moveTo(int position) { st.moveTo(position); }

	void run() { st.run(); }

  private:
	AccelStepper &st;
	IStepperDriver &drv;
};
#endif