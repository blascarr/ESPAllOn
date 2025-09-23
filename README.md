# ESPAllOn

Sistema de gestión y control para ESP32/ESP8266 con interfaz web basada en ESPUI.

## Configuración del Sistema de Archivos

El proyecto soporta dos modos de operación que se controlan mediante el define `USE_LITTLEFS_MODE` en `src/config.h`:

### Modo Estándar (por defecto)

```cpp
// #define USE_LITTLEFS_MODE  // Comentado = modo estándar
```

**Características:**

- ESPUI sirve archivos CSS/JS desde PROGMEM (memoria interna)
- Menor uso del sistema de archivos flash
- Endpoint `/pin-status` **NO disponible**
- Inicio más rápido
- Modo recomendado para desarrollo y uso básico

### Modo LittleFS

```cpp
#define USE_LITTLEFS_MODE  // Descomentado = modo filesystem
```

**Características:**

- ESPUI sirve archivos CSS/JS desde LittleFS (sistema de archivos)
- Endpoint `/pin-status` **disponible** con monitoreo completo de pines
- CSS personalizable editando `data/pin-status.css`
- Requiere subir sistema de archivos al ESP

**Configuración requerida:**

1. Descomenta `#define USE_LITTLEFS_MODE` en `src/config.h`
2. Sube el sistema de archivos: `pio run --target uploadfs --environment esp8266` (o `esp32`)
3. Compila y sube firmware: `pio run --target upload --environment esp8266` (o `esp32`)

## Endpoints Disponibles

**Modo Estándar:**

- `/` - Interfaz principal ESPUI

**Modo LittleFS:**

- `/` - Interfaz principal ESPUI
- `/pin-status` - Monitor detallado de estado de pines GPIO
- `/pin-status.css` - Hoja de estilos personalizable

## Desarrollo

Para cambiar entre modos:

1. Edita `USE_LITTLEFS_MODE` en `src/config.h`
2. Si activas LittleFS, ejecuta `pio run --target uploadfs --environment [esp8266|esp32]` antes del primer uso
3. Compila y sube con `pio run --target upload --environment [esp8266|esp32]`

**Nota:** El archivo `data/pin-status.css` se mantiene para futuras activaciones del modo LittleFS.

## Estructura de Archivos

```
data/                    # Archivos del sistema LittleFS
├── css/                 # Archivos CSS de ESPUI
├── js/                  # Archivos JavaScript de ESPUI
├── index.htm            # Página principal de ESPUI
└── pin-status.css       # Estilos del monitor de pines
```
