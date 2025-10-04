# 🚀 ESPAllOn - Plataforma Educativa para Proyectos IoT

**ESPAllOn** es una plataforma educativa diseñada específicamente para adolescentes que quieren desarrollar proyectos de Internet de las Cosas (IoT) sin necesidad de conocimientos avanzados en programación. Con una interfaz web intuitiva, permite configurar y controlar dispositivos ESP32/ESP8266 de forma visual y sencilla.

## 🎯 ¿Qué es ESPAllOn?

ESPAllOn es un sistema de gestión y control para placas ESP32/ESP8266 que incluye:

- **Interfaz web moderna** basada en ESPUI para configuración visual
- **Módulos predefinidos** (ESPinners) para diferentes tipos de hardware
- **Importación de proyectos** desde fuentes externas con un solo clic
- **Configuración automática** de pines y componentes
- **Control remoto** desde cualquier dispositivo con navegador web

## 🌟 Características Principales

### 🔧 Módulos Disponibles (ESPinners)

El sistema incluye módulos predefinidos para los componentes más comunes:

- **GPIO**: Entradas y salidas digitales básicas
- **Stepper**: Motores paso a paso con múltiples drivers (TMC2208, TMC2209, A4988, etc.)
- **DC Motor**: Motores de corriente continua con control de dirección
- **NeoPixel**: Tiras de LEDs direccionables
- **LCD**: Pantallas de cristal líquido
- **TFT**: Pantallas táctiles
- **RFID**: Lectores de tarjetas RFID
- **MPU**: Sensores de movimiento (acelerómetro/giroscopio)
- **Encoder**: Encoders rotativos

### 🌐 Interfaz Web

- **Configuración visual**: Selecciona pines y componentes desde el navegador
- **Control en tiempo real**: Activa/desactiva componentes desde cualquier dispositivo
- **Monitoreo de estado**: Visualiza el estado de todos los pines
- **Gestión de proyectos**: Importa configuraciones predefinidas

### 📡 Conectividad

- **WiFi integrado**: Conexión automática a redes WiFi
- **API REST**: Comunicación con servicios externos
- **Importación de proyectos**: Descarga configuraciones desde servidor remoto

## 🚀 Cómo Funciona

### 1. **Configuración Inicial**

1. Conecta tu ESP32/ESP8266 a la red WiFi
2. Accede a la interfaz web desde cualquier navegador
3. Configura los pines y componentes que necesites

### 2. **Uso de Módulos**

- Selecciona el tipo de componente (GPIO, Stepper, etc.)
- Asigna los pines correspondientes
- Configura parámetros específicos (velocidad, dirección, etc.)
- Guarda la configuración

### 3. **Control Remoto**

- Accede a la interfaz web desde cualquier dispositivo
- Controla componentes en tiempo real
- Monitorea el estado de sensores
- Ajusta parámetros dinámicamente

### 4. **Importación de Proyectos**

- Navega a la sección "Proyectos"
- Selecciona un proyecto desde la biblioteca online
- Carga la configuración con un solo clic
- ¡Tu proyecto está listo para usar!

## 🎓 Valor Educativo

### Para Estudiantes

- **Aprendizaje visual**: Configuración sin código
- **Conceptos IoT**: Comprende la interacción entre hardware y software
- **Proyectos reales**: Desarrolla soluciones prácticas
- **Escalabilidad**: Desde proyectos simples hasta sistemas complejos

### Para Educadores

- **Currículo estructurado**: Progresión desde conceptos básicos
- **Proyectos predefinidos**: Biblioteca de ejemplos
- **Evaluación práctica**: Resultados tangibles y medibles
- **Flexibilidad**: Adaptable a diferentes niveles educativos

## 🛠️ Configuración del Proyecto

### Requisitos

- **Hardware**: ESP32 o ESP8266
- **Software**: PlatformIO IDE
- **Conectividad**: Red WiFi

### Instalación

1. Clona el repositorio
2. Abre el proyecto en PlatformIO
3. Configura las credenciales WiFi en `src/config.h`
4. Compila y sube el firmware
5. Accede a la interfaz web

### Modos de Operación

#### Modo Estándar (Recomendado para principiantes)

```cpp
// #define USE_LITTLEFS_MODE  // Comentado = modo estándar
```

- Interfaz más rápida
- Menor uso de memoria
- Ideal para desarrollo y aprendizaje

#### Modo LittleFS (Para proyectos avanzados)

```cpp
#define USE_LITTLEFS_MODE  // Descomentado = modo filesystem
```

- Monitor de pines avanzado
- Personalización de estilos
- Funcionalidades adicionales

## 🔗 Integraciones

### API Externa

- **Servidor de proyectos**: `https://espallon.blascarr.com`
- **Biblioteca de configuraciones**: Proyectos predefinidos
- **Actualizaciones automáticas**: Nuevos módulos y funcionalidades

### Protocolos Soportados

- **HTTP/HTTPS**: Comunicación web estándar
- **JSON**: Intercambio de datos estructurado
- **REST API**: Integración con servicios externos

## 🎯 Casos de Uso Educativos

### En el Aula

- **Demostraciones interactivas**: Conceptos de IoT en tiempo real
- **Proyectos colaborativos**: Múltiples estudiantes trabajando en equipo
- **Evaluación práctica**: Resultados medibles y tangibles

### En Casa

- **Proyectos personales**: Automatización del hogar
- **Experimentación libre**: Prueba diferentes configuraciones
- **Compartir creaciones**: Comunidad de proyectos

## 🚀 Próximos Pasos

1. **Explora la interfaz**: Familiarízate con los controles
2. **Prueba un módulo simple**: Comienza con GPIO
3. **Importa un proyecto**: Usa la biblioteca online
4. **Crea tu primer proyecto**: Combina diferentes módulos
5. **Comparte tu experiencia**: Contribuye a la comunidad

## 📖 Documentación Adicional

- **Guía de inicio rápido**: Primeros pasos con ESPAllOn
- **Manual de módulos**: Detalles de cada ESPinner
- **API Reference**: Documentación técnica completa
- **Proyectos de ejemplo**: Tutoriales paso a paso

---

**ESPAllOn** - Haciendo la programación IoT accesible para todos 🎓✨

_¿Listo para crear tu primer proyecto IoT? ¡Comienza ahora!_
