#ifndef _ESPALLON_CONFIG_H
#define _ESPALLON_CONFIG_H

#include <Arduino.h>

#if defined(ESP32)
#include <ESPmDNS.h>
#include <WiFi.h>
#else
// esp8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <umm_malloc/umm_heap_select.h>
#define DEBUG_ESP_OOM

#ifndef MMU_IRAM_HEAP
#warning Try MMU option '2nd heap shared' in 'tools' IDE menu (cf. https://arduino-esp8266.readthedocs.io/en/latest/mmu.html#option-summary)
#warning use decorators: { HeapSelectIram doAllocationsInIRAM; ESPUI.addControl(...) ... } (cf. https://arduino-esp8266.readthedocs.io/en/latest/mmu.html#how-to-select-heap)
#warning then check http://<ip>/heap
#endif // MMU_IRAM_HEAP
#ifndef DEBUG_ESP_OOM
#error on ESP8266 and ESPUI, you must define OOM debug option when developping
#endif
#endif

#define VERBOSE false
#define MEMORYDEBUG false

// Settings
#define SLOW_BOOT 0
#define HOSTNAME "ESPAllOn"
#define FORCE_USE_HOTSPOT 0
#define DEBUG false

#define HARDCODED_CREDENTIALS true
#define HARDCODED_SSID "ZMS"
#define HARDCODED_PASS "ZM4K3RS:P"

#define DNS_NAME "espallon"

const byte DNS_PORT = 53;
IPAddress LOCAL_IP(192, 168, 1, 115);
IPAddress GATEWAY(192, 168, 1, 1);
IPAddress SUBNET(255, 255, 255, 0);
IPAddress PRIMARYDNS(9, 9, 9, 9);
IPAddress SECONDARYDNS(208, 67, 222, 222);

// ------------ Persistance CONFIG ------------//
String ESPinner_File = "/ESPinner";
String ESPinner_Path = "espinners";
#define ESPINNER_MODEL_JSONCONFIG "ESPinner_Mod"
#define ESPINNER_ID_JSONCONFIG "ID"

// -------- Persistance GPIO CONFIG --------//
#define ESPINNER_GPIO_JSONCONFIG "ESPINNER_GPIO"
#define ESPINNER_INPUT_CONFIG "INPUT"
#define ESPINNER_OUTPUT_CONFIG "OUTPUT"
#define ESPINNER_IO_JSONCONFIG "IO"

// -------- Persistance STEPPER CONFIG --------//
#define ESPINNER_STEPPER_JSONCONFIG "ESPINNER_STEPPER"
#define ESPINNER_STEPPER_DIR_CONFIG "DIR"
#define ESPINNER_STEPPER_STEP_CONFIG "STEP"
#define ESPINNER_STEPPER_EN_CONFIG "EN"
#define ESPINNER_STEPPER_CS_CONFIG "CS"
#define ESPINNER_STEPPER_DIAG0_CONFIG "DIAG0"
#define ESPINNER_STEPPER_DIAG1_CONFIG "DIAG1"
#define ESPINNER_STEPPER_ISSPI_CONFIG "ISSPI"
#define ESPINNER_STEPPER_ISDIAG_CONFIG "ISDIAG"
#define ESPINNER_STEPPER_DRIVER_CONFIG "DRIVER"
#define ESPINNER_STEPPER_STEPSREV_CONFIG "STEPS_PER_REV"

// -------- Persistance DC CONFIG --------//
#define ESPINNER_DC_JSONCONFIG "ESPINNER_DC"
#define ESPINNER_DCA_JSONCONFIG "ESPINNER_DCA"
#define ESPINNER_DCB_JSONCONFIG "ESPINNER_DCB"

// -------- Persistance DC CONFIG --------//
#define ESPINNER_NEOPIXEL_JSONCONFIG "ESPINNER_NEOPIXEL"

// -------- Persistance DC CONFIG --------//
#define ESPINNER_MPU_JSONCONFIG "ESPINNER_MPU"

// -------- Persistance DC CONFIG --------//
#define ESPINNER_ENCODER_JSONCONFIG "ESPINNER_ENCODER"

// -------- Persistance RFID CONFIG --------//
#define ESPINNER_RFID_JSONCONFIG "ESPINNER_RFID"

// -------- Persistance TFT CONFIG --------//
#define ESPINNER_TFT_JSONCONFIG "ESPINNER_TFT"

// -------- Persistance LCD CONFIG --------//
#define ESPINNER_LCD_JSONCONFIG "ESPINNER_LCD"

// --------------- TABS Definition ----------------//
#define BASICTAB_LABEL "ESPinners"
#define CONTROLLERTAB_LABEL "Controllers"
#define LINKACTIONSTAB_LABEL "Link Actions"
#define ADVANCEDSETTINGSTAB_LABEL "Advanced Settings"

#define ADVCONFIGTAB_LABEL "Config"
#define WIFITAB_LABEL "Wifi Credentials"

// ----------------------------------------//
// --------------- Modules ----------------//
// ----------------------------------------//

#define MOD_CAPACITY 10

#define ESPINNERTYPE_LABEL "ESPinnerType"
#define ESPINNERTYPE_VALUE "VOID"
#define ESPINNERID_LABEL "ESPinnerID"
#define ESPINNERID_VALUE "ID"
#define SAVEESPINNER_LABEL "ESPinner_Save"
#define SAVEESPINNER_VALUE "Save"
#define REMOVEESPINNER_LABEL "ESPinner_Remove"
#define REMOVEESPINNER_VALUE "Remove"
#define SSID_LABEL "SSID"
#define PASS_LABEL "Password"
#define SAVE_LABEL "Save"
#define VOID_LABEL "VOID"
#define SAVE_VALUE "Save"
#define VOID_VALUE ""

#define REMOVE_PINCONFIG_LABEL "REMOVE_PIN_CONFIGURATION"
#define REMOVE_PINCONFIG_VALUE "REMOVE PIN CONFIG"

#define ACTION_LABEL "Action"
#define ACTION_VALUE "Action"
#define LINKACTION_LABEL "Link"
#define LINKACTION_VALUE "Link"

// --------------- GPIO MOD Definition ----------------//
#define GPIO_LABEL "GPIO"
#define GPIO_ESPINNERINPUT_LABEL "INPUT"
#define GPIO_ESPINNEROUTPUT_LABEL "OUTPUT"
#define GPIO_SAVE_LABEL "GPIO_Save"
#define GPIO_SAVE_VALUE "Save GPIO"
#define GPIO_REMOVE_LABEL "GPIO_Remove"
#define GPIO_REMOVE_VALUE "Remove"
#define GPIO_SELECT_LABEL "Select_GPIO"
#define GPIO_SELECT_VALUE "Select"
#define GPIO_PINSELECTOR_LABEL "GPIO_PinSelector"
#define GPIO_PINSELECTOR_VALUE "0"
#define GPIO_MODESELECTOR_LABEL "GPIO_ModeSelector"

#define GPIO_ID_LABEL "GPIO_ID_Switcher"
#define GPIO_SWITCH_LABEL "GPIO_Switcher"
#define GPIO_SWITCH_VALUE "0"

// ------------- Stepper MOD Definition ---------------//
#define STEPPER_LABEL "Stepper"
#define STEPPER_SAVE_LABEL "Stepper_Save"
#define STEPPER_SAVE_VALUE "Save Stepper"
#define STEPPER_REMOVE_LABEL "Stepper_Remove"
#define STEPPER_REMOVE_VALUE "Remove"
#define STEPPER_DIR_SELECT_LABEL "Select_DIR_Stepper"
#define STEPPER_DIR_SELECT_VALUE "Select DIR Stepper"
#define STEPPER_STEP_SELECT_LABEL "Select_STEP_Stepper"
#define STEPPER_STEP_SELECT_VALUE "Select STEP Stepper"
#define STEPPER_EN_SELECT_LABEL "Select_EN_Stepper"
#define STEPPER_EN_SELECT_VALUE "Select EN Stepper"
#define STEPPER_CS_SELECT_LABEL "Select_CS_Stepper"
#define STEPPER_CS_SELECT_VALUE "Select CS Stepper"
#define STEPPER_DIAG0_SELECT_LABEL "Select_DIAG0_Stepper"
#define STEPPER_DIAG0_SELECT_VALUE "Select DIAG0 Stepper"
#define STEPPER_DIAG1_SELECT_LABEL "Select_DIAG1_Stepper"
#define STEPPER_DIAG1_SELECT_VALUE "Select DIAG1 Stepper"

#define STEPPER_DIR_SELECTOR_LABEL "Select_STEPPER_DIR"
#define STEPPER_DIR_SELECTOR_VALUE "0"
#define STEPPER_STEP_SELECTOR_LABEL "Select_STEPPER_STEP"
#define STEPPER_STEP_SELECTOR_VALUE "0"
#define STEPPER_EN_SELECTOR_LABEL "Select_STEPPER_EN"
#define STEPPER_EN_SELECTOR_VALUE "0"
#define STEPPER_CS_SELECTOR_LABEL "Select_STEPPER_CS"
#define STEPPER_CS_SELECTOR_VALUE "0"
#define STEPPER_DIAG0_SELECTOR_LABEL "Select_STEPPER_DIAG0"
#define STEPPER_DIAG0_SELECTOR_VALUE "0"
#define STEPPER_DIAG1_SELECTOR_LABEL "Select_STEPPER_DIAG1"
#define STEPPER_DIAG1_SELECTOR_VALUE "0"
#define STEPPER_MODESELECTOR_LABEL "Stepper_ModeSelector"

#define STEPPER_ID_LABEL "STEPPER_CONTROLLER"
#define STEPPER_SWITCH_EN_LABEL "Enable"
#define STEPPER_SWITCH_EN_VALUE "1"
#define STEPPER_SLIDER_VEL_LABEL "Vel:"
#define STEPPER_SLIDER_VEL_VALUE "50"
#define STEPPER_SLIDER_TARGET_LABEL "Length"
#define STEPPER_SLIDER_TARGET_VALUE "0"
#define STEPPER_PAD_MOVEMENT_LABEL "Stepper Pad"
#define STEPPER_PAD_MOVEMENT_VALUE "0"

#define STEPPER_LABEL_TARGET_LABEL "Target:"
#define STEPPER_LABEL_TARGET_VALUE "Target:"
#define STEPPER_LABEL_POSITION_LABEL "Current Position:"
#define STEPPER_LABEL_POSITION_VALUE "Current Position:"

#define STEPPER_LABEL_EN_LABEL "Enable"
#define STEPPER_LABEL_EN_VALUE "ON/OFF"
#define STEPPER_LABEL_VEL_LABEL "Vel:"
#define STEPPER_LABEL_VEL_VALUE "Vel:"

#define STEPPER_STEPSREV_LABEL "steps/rev"
#define STEPPER_STEPSREV_VALUE "200"

#define STEPPER_STEPSREV_MAX_VALUE 10000

#define STEPPER_MAX_STEPPERS 8
#define STEPPER_ACTIONS_INTERVAL_MS 100
// --------------- DC MOD Definition ----------------//
#define DC_LABEL "DC"
#define DC_ESPINNERINPUT_LABEL "INPUT"
#define DC_ESPINNEROUTPUT_LABEL "OUTPUT"
#define DC_SAVE_LABEL "DC_Save"
#define DC_SAVE_VALUE "Save DC"
#define DC_REMOVE_LABEL "DC_Remove"
#define DC_REMOVE_VALUE "Remove"
#define DC_PINA_SELECT_LABEL "Select_DC_PINA"
#define DC_PINA_SELECT_VALUE "Select DC PIN A"
#define DC_PINB_SELECT_LABEL "Select_DC_PINB"
#define DC_PINB_SELECT_VALUE "Select DC PIN B"
#define DC_PININPUT_LABEL "DC_PinInput"
#define DC_PININPUT_VALUE "Select_DC"
#define DC_PINA_SELECTOR_LABEL "DC_PINA_Selector"
#define DC_PINA_SELECTOR_VALUE "0"
#define DC_PINB_SELECTOR_LABEL "DC_PINB_Selector"
#define DC_PINB_SELECTOR_VALUE "0"
#define DC_MODESELECTOR_LABEL "DC_ModeSelector"

#define DC_ID_LABEL "DC_CONTROLLER"
#define DC_SWITCH_DIR_LABEL "DC_Switcher"
#define DC_SWITCH_DIR_VALUE "0"
#define DC_SWITCH_RUN_LABEL "STOP"
#define DC_SWITCH_RUN_VALUE "0"
#define DC_SLIDER_VEL_LABEL "DC_VEL"
#define DC_SLIDER_VEL_VALUE "50"

// ------------- NeoPixel MOD Definition --------------//
#define NEOPIXEL_LABEL "Neopixel"
#define NEOPIXEL_ESPINNERINPUT_LABEL "INPUT"
#define NEOPIXEL_ESPINNEROUTPUT_LABEL "OUTPUT"
#define NEOPIXEL_SAVE_LABEL "Neopixel_Save"
#define NEOPIXEL_SAVE_VALUE "Save Neopixel"
#define NEOPIXEL_REMOVE_LABEL "Neopixel_Remove"
#define NEOPIXEL_REMOVE_VALUE "Remove"
#define NEOPIXEL_SELECT_LABEL "Select_Neopixel"
#define NEOPIXEL_SELECT_VALUE "Select Neopixel"
#define NEOPIXEL_PININPUT_LABEL "Neopixel_PinInput"
#define NEOPIXEL_PININPUT_VALUE "Select_Neopixel"
#define NEOPIXEL_PINSELECTOR_LABEL "Neopixel_PinSelector"
#define NEOPIXEL_PINSELECTOR_VALUE "0"
#define NEOPIXEL_MODESELECTOR_LABEL "Neopixel_ModeSelector"

// --------------- LCD MOD Definition -----------------//
#define LCD_LABEL "LCD"
#define LCD_ESPINNERINPUT_LABEL "INPUT"
#define LCD_ESPINNEROUTPUT_LABEL "OUTPUT"
#define LCD_SAVE_LABEL "LCD_Save"
#define LCD_SAVE_VALUE "Save LCD"
#define LCD_REMOVE_LABEL "LCD_Remove"
#define LCD_REMOVE_VALUE "Remove"
#define LCD_SELECT_LABEL "Select_LCD"
#define LCD_SELECT_VALUE "Select LCD"
#define LCD_PININPUT_LABEL "LCD_PinInput"
#define LCD_PININPUT_VALUE "Select_LCD"
#define LCD_PINSELECTOR_LABEL "LCD_PinSelector"
#define LCD_PINSELECTOR_VALUE "0"
#define LCD_MODESELECTOR_LABEL "LCD_ModeSelector"

// --------------- RFID MOD Definition ----------------//
#define RFID_LABEL "RFID"
#define RFID_ESPINNERINPUT_LABEL "INPUT"
#define RFID_ESPINNEROUTPUT_LABEL "OUTPUT"
#define RFID_SAVE_LABEL "RFID_Save"
#define RFID_SAVE_VALUE "Save RFID"
#define RFID_REMOVE_LABEL "RFID_Remove"
#define RFID_REMOVE_VALUE "Remove"
#define RFID_SELECT_LABEL "Select_RFID"
#define RFID_SELECT_VALUE "Select RFID"
#define RFID_PININPUT_LABEL "RFID_PinInput"
#define RFID_PININPUT_VALUE "Select_RFID"
#define RFID_PINSELECTOR_LABEL "RFID_PinSelector"
#define RFID_PINSELECTOR_VALUE "0"
#define RFID_MODESELECTOR_LABEL "RFID_ModeSelector"

// ---------------- MPU MOD Definition ----------------//
#define MPU_LABEL "MPU"
#define MPU_ESPINNERINPUT_LABEL "INPUT"
#define MPU_ESPINNEROUTPUT_LABEL "OUTPUT"
#define MPU_SAVE_LABEL "MPU_Save"
#define MPU_SAVE_VALUE "Save MPU"
#define MPU_REMOVE_LABEL "MPU_Remove"
#define MPU_REMOVE_VALUE "Remove"
#define MPU_SELECT_LABEL "Select_MPU"
#define MPU_SELECT_VALUE "Select MPU"
#define MPU_PININPUT_LABEL "MPU_PinInput"
#define MPU_PININPUT_VALUE "Select_MPU"
#define MPU_PINSELECTOR_LABEL "MPU_PinSelector"
#define MPU_PINSELECTOR_VALUE "0"
#define MPU_MODESELECTOR_LABEL "MPU_ModeSelector"

// ---------------- TFT MOD Definition ----------------//
#define TFT_LABEL "TFT"
#define TFT_ESPINNERINPUT_LABEL "INPUT"
#define TFT_ESPINNEROUTPUT_LABEL "OUTPUT"
#define TFT_SAVE_LABEL "TFT_Save"
#define TFT_SAVE_VALUE "Save TFT"
#define TFT_REMOVE_LABEL "TFT_Remove"
#define TFT_REMOVE_VALUE "Remove"
#define TFT_SELECT_LABEL "Select_TFT"
#define TFT_SELECT_VALUE "Select TFT"
#define TFT_PININPUT_LABEL "TFT_PinInput"
#define TFT_PININPUT_VALUE "Select_TFT"
#define TFT_PINSELECTOR_LABEL "TFT_PinSelector"
#define TFT_PINSELECTOR_VALUE "0"
#define TFT_MODESELECTOR_LABEL "TFT_ModeSelector"

// ------------- Encoder MOD Definition ---------------//
#define ENCODER_LABEL "Encoder"
#define ENCODER_ESPINNERINPUT_LABEL "INPUT"
#define ENCODER_ESPINNEROUTPUT_LABEL "OUTPUT"
#define ENCODER_SAVE_LABEL "Encoder_Save"
#define ENCODER_SAVE_VALUE "Save Encoder"
#define ENCODER_REMOVE_LABEL "Encoder_Remove"
#define ENCODER_REMOVE_VALUE "Remove"
#define ENCODER_SELECT_LABEL "Select_Encoder"
#define ENCODER_SELECT_VALUE "Select Encoder"
#define ENCODER_PININPUT_LABEL "Encoder_PinInput"
#define ENCODER_PININPUT_VALUE "Select_Encoder"
#define ENCODER_PINSELECTOR_LABEL "Encoder_PinSelector"
#define ENCODER_PINSELECTOR_VALUE "0"
#define ENCODER_MODESELECTOR_LABEL "Encoder_ModeSelector"

#endif