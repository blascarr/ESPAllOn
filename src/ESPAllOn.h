#include "ESPinner.h"
#include <ESPUI.h>

void setUpUI();

void generalCallback(Control *sender, int type);
void extendedCallback(Control *sender, int type, void *param);

uint16_t mainLabel, mainSwitcher, mainSlider, mainText, mainNumber,
	mainScrambleButton, mainTime;
volatile bool updates = false;
auto maintab = ESPUI.addControl(Tab, "", "Basic controls");

void setUpUI() {

#ifdef ESP8266
	{
		HeapSelectIram doAllocationsInIRAM;
#endif

		ESPUI.addControl(Separator, "General controls", "", None, maintab);
		ESPUI.addControl(Button, "Button", "Button 1", Alizarin, maintab,
						 extendedCallback, (void *)19);
		mainLabel = ESPUI.addControl(Label, "Label", "Label text", Emerald,
									 maintab, generalCallback);
		ESPUI.begin(HOSTNAME);

#ifdef ESP8266
	} // HeapSelectIram
#endif
}

void generalCallback(Control *sender, int type) {
	Serial.print("CB: id(");
	Serial.print(sender->id);
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
}

void extendedCallback(Control *sender, int type, void *param) {
	Serial.print("CB: id(");
	Serial.print(sender->id);
	Serial.print(") Type(");
	Serial.print(type);
	Serial.print(") '");
	Serial.print(sender->label);
	Serial.print("' = ");
	Serial.println(sender->value);
	Serial.println(String("param = ") + String((int)param));
}
