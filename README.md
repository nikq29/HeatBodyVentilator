# ESP32 Server Project Core

Ein ESP32-basiertes Webserver-Projekt mit MQTT-Integration für Home Assistant.

## Features

- 🌐 **Web Interface**: Responsive Weboberfläche mit Hamburger-Menü
- 📡 **MQTT Integration**: Home Assistant Auto Discovery Support
- 💡 **LED Control**: FastLED-basierte LED-Steuerung
- 🔐 **Authentication**: Token-basierte Authentifizierung
- 📱 **Responsive Design**: Mobile-optimierte Benutzeroberfläche
- ⚙️ **Configuration**: WiFi und MQTT Einstellungen über Web-Interface

## Hardware

- ESP32 M5Stack Atom
- Integrierte LED (FastLED)

## Software Stack

- **Platform**: PlatformIO
- **Framework**: Arduino
- **Libraries**: 
  - FastLED
  - WebServer
  - WiFi
  - Preferences
  - ArduinoJson

## Project Structure

```
├── data/                   # Web files (HTML, CSS, Images)
│   ├── index.html         # Login page
│   ├── main.html          # Main dashboard
│   ├── setting.html       # Settings page
│   └── style.css          # Unified styles
├── src/                   # Source code
│   ├── main.cpp           # Main application
│   ├── ServerManager.*    # Web server management
│   ├── WiFiManager.*      # WiFi connection handling
│   ├── LEDManager.*       # LED control
│   ├── MQTTManager.*      # MQTT functionality
│   └── Config.*           # Configuration management
└── platformio.ini         # PlatformIO configuration
```

## Getting Started

1. Clone this repository
2. Open in PlatformIO (VS Code with PlatformIO extension)
3. Build and upload to your ESP32 device
4. Connect to the device's WiFi AP for initial setup
5. Configure WiFi and MQTT settings through web interface

## Web Interface

The web interface provides:
- **Home**: Main dashboard with device status
- **Settings**: WiFi and MQTT configuration
- **LED Control**: Toggle device LED
- **Logout**: Secure session management

## MQTT Integration

Supports Home Assistant Auto Discovery with:
- Device registration
- LED state control
- Status reporting

## Development

Built with:
- PlatformIO for embedded development
- Modern C++ practices
- Modular architecture
- Token-based security

## License

Private Project - All Rights Reserved

## Version History

See git commit history for detailed version information.
