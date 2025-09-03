#include "WiFiManager.h"
#include <EEPROM.h>
#include <Preferences.h>

WiFiManager wifi; // Nur HIER die Definition!

void WiFiManager::beginAP() {
    WiFi.mode(WIFI_AP_STA); // Dual Mode aktivieren
    WiFi.softAP(Config::AP_SSID, Config::AP_PASS);
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
        WiFi.mode(WIFI_AP_STA);
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
        }
    } else {
        Serial.println("No saved WiFi credentials found.");
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
            WiFi.mode(WIFI_AP_STA);
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
                Serial.println("Reconnect failed. Staying in dual mode (AP + STA)");
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

