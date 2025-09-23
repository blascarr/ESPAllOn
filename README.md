# ESPAllOn

Management and control system for ESP32/ESP8266 with ESPUI-based web interface.

## File System Configuration

The project supports two operating modes controlled by the `USE_LITTLEFS_MODE` define in `src/config.h`:

### Standard Mode (default)

```cpp
// #define USE_LITTLEFS_MODE  // Commented = standard mode
```

**Features:**

- ESPUI serves CSS/JS files from PROGMEM (internal memory)
- Lower flash filesystem usage
- `/pin-status` endpoint **NOT available**
- Faster startup
- Recommended mode for development and basic usage

### LittleFS Mode

```cpp
#define USE_LITTLEFS_MODE  // Uncommented = filesystem mode
```

**Features:**

- ESPUI serves CSS/JS files from LittleFS (file system)
- `/pin-status` endpoint **available** with comprehensive pin monitoring
- Customizable CSS by editing `data/pin-status.css`
- Requires uploading file system to ESP

**Required configuration:**

1. Uncomment `#define USE_LITTLEFS_MODE` in `src/config.h`
2. Upload file system: `pio run --target uploadfs --environment esp8266` (or `esp32`)
3. Compile and upload firmware: `pio run --target upload --environment esp8266` (or `esp32`)

## Available Endpoints

**Standard Mode:**

- `/` - Main ESPUI interface

**LittleFS Mode:**

- `/` - Main ESPUI interface
- `/pin-status` - Detailed GPIO pin status monitor
- `/pin-status.css` - Customizable stylesheet

## Development

To switch between modes:

1. Edit `USE_LITTLEFS_MODE` in `src/config.h`
2. If activating LittleFS, run `pio run --target uploadfs --environment [esp8266|esp32]` before first use
3. Compile and upload with `pio run --target upload --environment [esp8266|esp32]`

**Note:** The `data/pin-status.css` file is maintained for future activations of LittleFS mode.

## File Structure

```
data/                    # LittleFS file system files
├── css/                 # ESPUI CSS files
├── js/                  # ESPUI JavaScript files
├── index.htm            # ESPUI main page
└── pin-status.css       # Pin monitor styles
```
