#include <Preferences.h>
#include "Config.h"
#include <time.h>

namespace Config {
    char WEB_PASSWORD[32] = "your_password_here";
    char DEVICE_NAME[32] = "ESP32-Test";
    char MQTT_SERVER[64] = "";
    uint16_t MQTT_PORT = 1883;
    char MQTT_USER[32] = "";
    char MQTT_PASS[32] = "";
    char MQTT_TOPIC[64] = "home/esp32";
    char TEMP_UNIT[16] = "celsius";
    bool AP_ENABLED = true;
    char LAST_PASSWORD_CHANGE[32] = "Nie geändert";

    void loadSettings() {
        Preferences prefs;
        if (!prefs.begin("settings", true)) {
            Serial.println("Warning: Failed to open settings preferences, using defaults");
            return;
        }
        
        String pw = prefs.getString("webpw", "your_password_here");
        strncpy(WEB_PASSWORD, pw.c_str(), sizeof(WEB_PASSWORD));
        WEB_PASSWORD[sizeof(WEB_PASSWORD)-1] = '\0';
        
        String deviceName = prefs.getString("device_name", "ESP32-Test");
        strncpy(DEVICE_NAME, deviceName.c_str(), sizeof(DEVICE_NAME));
        DEVICE_NAME[sizeof(DEVICE_NAME)-1] = '\0';
        
        String mqttServer = prefs.getString("mqtt_server", "");
        strncpy(MQTT_SERVER, mqttServer.c_str(), sizeof(MQTT_SERVER));
        MQTT_SERVER[sizeof(MQTT_SERVER)-1] = '\0';
        
        MQTT_PORT = prefs.getUShort("mqtt_port", 1883);
        
        String mqttUser = prefs.getString("mqtt_user", "");
        strncpy(MQTT_USER, mqttUser.c_str(), sizeof(MQTT_USER));
        MQTT_USER[sizeof(MQTT_USER)-1] = '\0';
        
        String mqttPass = prefs.getString("mqtt_pass", "");
        strncpy(MQTT_PASS, mqttPass.c_str(), sizeof(MQTT_PASS));
        MQTT_PASS[sizeof(MQTT_PASS)-1] = '\0';
        Serial.print("Loaded MQTT password length: ");
        Serial.println(mqttPass.length());
        Serial.print("MQTT_PASS content: [");
        Serial.print(MQTT_PASS);
        Serial.println("]");
        
        String mqttTopic = prefs.getString("mqtt_topic", "home/esp32");
        strncpy(MQTT_TOPIC, mqttTopic.c_str(), sizeof(MQTT_TOPIC));
        MQTT_TOPIC[sizeof(MQTT_TOPIC)-1] = '\0';
        
        String tempUnit = prefs.getString("temp_unit", "celsius");
        strncpy(TEMP_UNIT, tempUnit.c_str(), sizeof(TEMP_UNIT));
        TEMP_UNIT[sizeof(TEMP_UNIT)-1] = '\0';
        
        AP_ENABLED = prefs.getBool("ap_enabled", true);
        
        String lastPwChange = prefs.getString("last_pw_change", "Nie geändert");
        strncpy(LAST_PASSWORD_CHANGE, lastPwChange.c_str(), sizeof(LAST_PASSWORD_CHANGE));
        LAST_PASSWORD_CHANGE[sizeof(LAST_PASSWORD_CHANGE)-1] = '\0';
        
        prefs.end();
        Serial.println("Settings loaded successfully");
    }

    void saveWebPassword(const char* newPassword) {
        Preferences prefs;
        if (!prefs.begin("settings", false)) {
            Serial.println("Error: Failed to open settings preferences for writing");
            return;
        }
        prefs.putString("webpw", newPassword);
        strncpy(WEB_PASSWORD, newPassword, sizeof(WEB_PASSWORD));
        WEB_PASSWORD[sizeof(WEB_PASSWORD)-1] = '\0';
        
        // Save timestamp
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        char timestamp[32];
        strftime(timestamp, sizeof(timestamp), "%d.%m.%Y %H:%M", timeinfo);
        prefs.putString("last_pw_change", timestamp);
        strncpy(LAST_PASSWORD_CHANGE, timestamp, sizeof(LAST_PASSWORD_CHANGE));
        LAST_PASSWORD_CHANGE[sizeof(LAST_PASSWORD_CHANGE)-1] = '\0';
        
        prefs.end();
    }

    void saveDeviceName(const char* newName) {
        Preferences prefs;
        if (!prefs.begin("settings", false)) {
            Serial.println("Error: Failed to open settings preferences for writing");
            return;
        }
        prefs.putString("device_name", newName);
        strncpy(DEVICE_NAME, newName, sizeof(DEVICE_NAME));
        DEVICE_NAME[sizeof(DEVICE_NAME)-1] = '\0';
        prefs.end();
    }

    void saveMQTTSettings(const char* server, uint16_t port, const char* user, const char* pass, const char* topic) {
        Preferences prefs;
        if (!prefs.begin("settings", false)) {
            Serial.println("Error: Failed to open settings preferences for writing");
            return;
        }
        prefs.putString("mqtt_server", server);
        prefs.putUShort("mqtt_port", port);
        prefs.putString("mqtt_user", user);
        prefs.putString("mqtt_pass", pass);
        Serial.print("Saving MQTT password: [");
        Serial.print(pass);
        Serial.println("]");
        Serial.print("Password length: ");
        Serial.println(strlen(pass));
        prefs.putString("mqtt_topic", topic);
        
        strncpy(MQTT_SERVER, server, sizeof(MQTT_SERVER));
        MQTT_SERVER[sizeof(MQTT_SERVER)-1] = '\0';
        MQTT_PORT = port;
        strncpy(MQTT_USER, user, sizeof(MQTT_USER));
        MQTT_USER[sizeof(MQTT_USER)-1] = '\0';
        strncpy(MQTT_PASS, pass, sizeof(MQTT_PASS));
        MQTT_PASS[sizeof(MQTT_PASS)-1] = '\0';
        strncpy(MQTT_TOPIC, topic, sizeof(MQTT_TOPIC));
        MQTT_TOPIC[sizeof(MQTT_TOPIC)-1] = '\0';
        
        prefs.end();
    }

    void saveTempUnit(const char* unit) {
        Preferences prefs;
        if (!prefs.begin("settings", false)) {
            Serial.println("Error: Failed to open settings preferences for writing");
            return;
        }
        prefs.putString("temp_unit", unit);
        strncpy(TEMP_UNIT, unit, sizeof(TEMP_UNIT));
        TEMP_UNIT[sizeof(TEMP_UNIT)-1] = '\0';
        prefs.end();
    }

    void saveAPEnabled(bool enabled) {
        Preferences prefs;
        if (!prefs.begin("settings", false)) {
            Serial.println("Error: Failed to open settings preferences for writing");
            return;
        }
        prefs.putBool("ap_enabled", enabled);
        AP_ENABLED = enabled;
        prefs.end();
    }

    void factoryReset() {
        Serial.println("Performing factory reset...");
        
        Preferences prefs;
        if (prefs.begin("settings", false)) {
            prefs.clear();
            prefs.end();
            Serial.println("Settings preferences cleared");
        } else {
            Serial.println("Warning: Failed to clear settings preferences");
        }
        
        if (prefs.begin("wifi", false)) {
            prefs.clear();
            prefs.end();
            Serial.println("WiFi preferences cleared");
        } else {
            Serial.println("Warning: Failed to clear WiFi preferences");
        }
        
        // Reset to defaults
        strcpy(WEB_PASSWORD, "your_password_here");
        strcpy(DEVICE_NAME, "ESP32-Test");
        strcpy(MQTT_SERVER, "");
        MQTT_PORT = 1883;
        strcpy(MQTT_USER, "");
        strcpy(MQTT_PASS, "");
        strcpy(MQTT_TOPIC, "home/esp32");
        strcpy(TEMP_UNIT, "celsius");
        AP_ENABLED = true;
        strcpy(LAST_PASSWORD_CHANGE, "Nie geändert");
        
        Serial.println("Factory reset completed");
    }

    String getLastPasswordChange() {
        return String(LAST_PASSWORD_CHANGE);
    }
}