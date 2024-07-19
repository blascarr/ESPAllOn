#ifndef _ESPALLON_PIN_MANAGER_H
#define _ESPALLON_PIN_MANAGER_H
#include "ESP_Boards.h"
#include <PinManager.h>

class ESPAllOnPinManager : public PinManager<ESP_BoardConf, PinMode> {

  public:
	ESPAllOnPinManager() {}
};
#endif