#include <Arduino.h>
#include "WiFiManager.h"
#include "ServerManager.h"
#include "LEDManager.h"
#include "MQTTManager.h"
#include "Config.h"
#include <Preferences.h>
#include <nvs_flash.h>

ServerManager web;
LEDManager led;
extern WiFiManager wifi; // Use the existing instance from WiFiManager.cpp
extern MQTTManager mqttManager; // Use the existing instance from MQTTManager.cpp
bool hasWifiCredentials = false;
unsigned long lastWifiCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 10000; // Check every 10 seconds (reduced from 30)

// LED callback function for MQTT
void onLEDStateChange(bool state) {
    if (state) {
        led.setColor(CRGB::White); // White for ON
    } else {
        led.setColor(CRGB::Black); // Black for OFF
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000); // Give serial time to initialize
    
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        Serial.println("Erasing NVS flash...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    Serial.println("NVS initialized successfully");
    
    led.begin();
    
    // Load all settings from preferences
    Config::loadSettings();
    
    // Check if WiFi credentials exist
    Preferences prefs;
    if (prefs.begin("wifi", true)) {
        String savedSsid = prefs.getString("ssid", "");
        hasWifiCredentials = (savedSsid.length() > 0);
        prefs.end();
        Serial.println("WiFi credentials " + String(hasWifiCredentials ? "found" : "not found"));
    } else {
        Serial.println("Failed to open WiFi preferences");
        hasWifiCredentials = false;
    }
    
    // Initialize WiFi based on AP_ENABLED setting
    Serial.println("DEBUG: Checking AP_ENABLED setting...");
    Serial.println("DEBUG: Config::AP_ENABLED = " + String(Config::AP_ENABLED ? "true" : "false"));
    
    if (Config::AP_ENABLED) {
        Serial.println("DEBUG: Starting dual mode (AP + STA)...");
        wifi.beginAP();
        Serial.println("Access Point started");
        // Also start STA mode for dual functionality
        wifi.beginAutoSTA();
        Serial.println("Starting in dual mode (Access Point + Station enabled)");
    } else {
        Serial.println("DEBUG: AP disabled, STA only mode...");
        // Ensure AP is stopped if disabled
        wifi.stopAP();
        Serial.println("AP mode disabled");
        // Only try STA mode if AP is disabled
        wifi.beginAutoSTA(); // Verbindet sich automatisch mit gespeicherter SSID/Passwort
    }
    
    web.begin();
    mqttManager.begin();
    
    // Register LED callback with MQTT manager
    mqttManager.setLEDCallback(onLEDStateChange);
    
    Serial.println("Setup completed");
}

void loop() {
    web.handleClient();
    mqttManager.loop();
    
    // Check WiFi connection periodically, not every loop iteration
    if (hasWifiCredentials && (millis() - lastWifiCheck > WIFI_CHECK_INTERVAL)) {
        wifi.checkWiFiConnection();
        lastWifiCheck = millis();
    }
    
    // Small delay to prevent watchdog issues
    delay(1);
}
