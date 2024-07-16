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

// Settings
#define SLOW_BOOT 0
#define HOSTNAME "ESPAllOn"
#define FORCE_USE_HOTSPOT 0
#define DEBUG true

#define HARDCODED_CREDENTIALS true
#define HARDCODED_SSID "ZMS"
#define HARDCODED_PASS "ZM4K3RS:P"

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 115);

// Modules

#define MOD_CAPACITY 10