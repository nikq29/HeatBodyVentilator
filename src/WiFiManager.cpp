#include "WiFiManager.h"
#include <EEPROM.h>
#include <Preferences.h>

WiFiManager wifi; // Nur HIER die Definition!

void WiFiManager::beginAP() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("ESP32-Setup", "12345678");
    Serial.println("Access Point started");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
}

void WiFiManager::stopAP() {
    Serial.println("Stopping Access Point...");
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    Serial.println("Access Point stopped - WiFi mode set to STA only");
}

void WiFiManager::beginSTA(const char* ssid, const char* pass) {
    WiFi.mode(WIFI_AP_STA); // Dual Mode aktivieren
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi!");
    Serial.println("Station IP Address: " + WiFi.localIP().toString());
    // TODO: Füge Timeout und Fallback hinzu
}

void WiFiManager::beginAutoSTA() {
    Preferences prefs;
    prefs.begin("wifi", true);
    String savedSsid = prefs.getString("ssid", "");
    String savedPassword = prefs.getString("password", "");
    prefs.end();
    
    if (savedSsid.length() > 0 && savedPassword.length() > 0) {
        // Start with STA mode first for clean connection attempt
        WiFi.mode(WIFI_STA);
        WiFi.begin(savedSsid.c_str(), savedPassword.c_str());
        
        unsigned long startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
            delay(500);
            Serial.println("Connecting to saved WiFi...");
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Connected to saved WiFi!");
            Serial.println("Station IP Address: " + WiFi.localIP().toString());
        } else {
            Serial.println("Could not connect to saved WiFi.");
            // Emergency fallback: Start AP for user access, regardless of AP_ENABLED setting
            Serial.println("Starting emergency Access Point for initial setup/troubleshooting...");
            WiFi.mode(WIFI_AP_STA);
            WiFi.softAP("ESP32-Setup", "12345678");
            Serial.println("Emergency AP started - connect to ESP32-Setup network");
            Serial.print("Emergency AP IP: ");
            Serial.println(WiFi.softAPIP());
        }
    } else {
        Serial.println("No saved WiFi credentials found. Starting in AP mode for initial setup.");
        beginAP(); // Use the regular AP start function
    }
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost. Status: " + String(WiFi.status()));
        Serial.println("Attempting to reconnect...");
        
        Preferences prefs;
        prefs.begin("wifi", true);
        String savedSsid = prefs.getString("ssid", "");
        String savedPassword = prefs.getString("password", "");
        prefs.end();
        
        if (savedSsid.length() > 0 && savedPassword.length() > 0) {
            WiFi.disconnect();
            delay(1000);
            WiFi.mode(WIFI_STA); // Start with STA only for reconnection attempt
            WiFi.begin(savedSsid.c_str(), savedPassword.c_str());
            
            unsigned long startAttemptTime = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
                delay(500);
                Serial.println("Reconnecting...");
            }
            
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("Reconnected to WiFi!");
                Serial.println("Station IP Address: " + WiFi.localIP().toString());
            } else {
                Serial.println("Reconnect failed. Starting emergency Access Point for user access...");
                // Emergency fallback: Start AP regardless of user setting
                WiFi.mode(WIFI_AP_STA);
                WiFi.softAP("ESP32-Setup", "12345678");
                Serial.println("Emergency AP started - user can access via ESP32-Setup network");
                Serial.print("Emergency AP IP: ");
                Serial.println(WiFi.softAPIP());
            }
        }
    } else {
        // Verbindung ist OK, gelegentlich IP-Adresse ausgeben
        static unsigned long lastIPOutput = 0;
        if (millis() - lastIPOutput > 60000) { // Alle 60 Sekunden
            Serial.println("WiFi connected. IP: " + WiFi.localIP().toString());
            lastIPOutput = millis();
        }
    }
}

