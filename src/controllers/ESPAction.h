#ifndef _ESPALLON_ACTION_H
#define _ESPALLON_ACTION_H

#include <Arduino.h>

/**
 * ESPAction encapsulates a user-defined function that can be triggered by
 * various events such as GPIO state changes, UI interactions, or external
 * triggers.
 */
class ESPAction {
  public:
	/** Function pointer type for action callbacks */
	typedef void (*ActionFunc)(uint16_t);

	/**
	 * @param name Human-readable name for the action
	 * @param reference Unique reference identifier for the action
	 * @param func Function pointer to execute when action is triggered
	 */
	ESPAction(const String &name, const String &reference, ActionFunc func)
		: name(name), reference(reference), action(func) {}

	/**
	 * Executes the action with the provided state
	 * @param state State parameter passed to the action function
	 */
	void execute(uint16_t state) { action(state); }

	/**
	 * Gets the human-readable name of the action
	 * @return Reference to the action name string
	 */
	const String &getName() const { return name; }

	/**
	 * Gets the unique reference identifier of the action
	 * @return Reference to the action reference string
	 */
	const String &getReference() const { return reference; }

  private:
	String name;
	String reference;
	ActionFunc action;
};
#endif