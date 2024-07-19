#include <Arduino.h>

class ESPAction {
  public:
	typedef void (*ActionFunc)(uint16_t);
	ESPAction(const String &name, const String &reference, ActionFunc func)
		: name(name), reference(reference), action(func) {}

	void execute(uint16_t state) { action(state); }

	const String &getName() const { return name; }
	const String &getReference() const { return reference; }

  private:
	String name;
	String reference;
	ActionFunc action;
};