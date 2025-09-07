#pragma once
#include <WebServer.h>
#include "Config.h"

class ServerManager {
public:
    ServerManager();
    void begin();
    void handleClient();
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
    
    bool isAuthenticated();
};
