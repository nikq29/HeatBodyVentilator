#pragma once
#include <WebServer.h>
#include "Config.h"
#include "KMeterManager.h"

class ServerManager {
public:
    ServerManager();
    void begin();
    void handleClient();
    void initializePWM(); // PWM-Initialisierung
    void setPWMDuty(int duty); // Öffentliche PWM-Steuerung für andere Funktionen
    void updateSensors(); // Sensor-Updates
    KMeterManager* getKMeterManager() { return &kmeterManager; } // Zugriff auf KMeter
private:
    WebServer server;
    void setupRoutes();
    void handleRoot();
    void handleMain();
    void handleIndex();
    void handleLogin();
    void handleNotFound();
    void handleSetting();
    void handleSettingWifi();
    void handleWifiConnect();
    
    // New API endpoints
    void handleDeviceName();
    void handleReboot();
    void handleFactoryReset();
    void handleChangePassword();
    void handleTempUnit();
    void handleToggleAP();
    void handleMQTTSettings();
    void handleMQTTTest();
    void handleMQTTStatus();
    void handleWiFiStatus();
    void handleWiFiScan();
    void handleWiFiDisconnect();
    void handleWiFiClear();
    void handleLEDToggle();
    void handleStatus();
    void handleGetSettings();
    void handleFirmwareUpdate();
    void handlePWMControl();
    void handlePWMStatus();
    void handleSystemInfo(); // Neue API für Platzhalter-Daten
    void handleKMeterStatus(); // KMeter Sensor Status
    void handleKMeterConfig(); // KMeter Konfiguration
    void handleTempMapping(); // Temperature mapping settings
    void handleTempMappingStatus(); // Temperature mapping status
    void handleAutoPWM(); // Auto PWM toggle
    
    bool isAuthenticated();
    
    // Temperature to PWM mapping functions
    int mapTemperatureToPWM(float temperature);
    void updateAutoPWM(); // Update PWM based on current temperature
    
    // Sensor Manager
    KMeterManager kmeterManager;
};
