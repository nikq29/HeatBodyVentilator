#include "ServerManager.h"
#include "Config.h"
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "LEDManager.h"
#include <WiFi.h> // oder #include <ESP8266WiFi.h> für ESP8266
#include <Preferences.h>

// External references
extern LEDManager led;
extern MQTTManager mqttManager;

extern const uint8_t _binary_data_index_html_start[] asm("_binary_data_index_html_start");
extern const uint8_t _binary_data_index_html_end[]   asm("_binary_data_index_html_end");
extern const uint8_t _binary_data_style_css_start[]  asm("_binary_data_style_css_start");
extern const uint8_t _binary_data_style_css_end[]    asm("_binary_data_style_css_end");
extern const uint8_t _binary_data_login_html_start[] asm("_binary_data_login_html_start");
extern const uint8_t _binary_data_login_html_end[]   asm("_binary_data_login_html_end");
extern const uint8_t _binary_data_img_logo_png_start[]   asm("_binary_data_img_logo_png_start");
extern const uint8_t _binary_data_img_logo_png_end[]     asm("_binary_data_img_logo_png_end");
extern const uint8_t _binary_data_main_html_start[] asm("_binary_data_main_html_start");
extern const uint8_t _binary_data_main_html_end[]   asm("_binary_data_main_html_end");
// Removed: setting_wifi.html - using modals in settings page instead
// extern const uint8_t _binary_data_setting_wifi_html_start[] asm("_binary_data_setting_wifi_html_start");
// extern const uint8_t _binary_data_setting_wifi_html_end[]   asm("_binary_data_setting_wifi_html_end");

String sessionToken;



ServerManager::ServerManager()
    : server(Config::HTTP_PORT) {}

void ServerManager::begin() {
    setupRoutes();
    server.begin();
    Serial.print("Web server started on port ");
    Serial.println(Config::HTTP_PORT);
}

void ServerManager::handleClient() {
    server.handleClient();
}

void ServerManager::setupRoutes() {
  // index.html
  server.on("/", HTTP_GET, [this]() {
    server.sendHeader("Location", "/main");
    server.send(302);
  });
  server.on("/main", HTTP_GET, [this]() { handleMain(); });
  // style.css
  server.on("/style.css", HTTP_GET, [this]() {
    size_t len = _binary_data_style_css_end - _binary_data_style_css_start;
    server.setContentLength(len);
    server.send(200, "text/css", "");
    server.sendContent_P(reinterpret_cast<const char*>(_binary_data_style_css_start), len);
  });
  // favicon, login etc. wie gehabt…
  server.on("/favicon.ico", HTTP_GET, [this]() { server.send(204); });
  server.on("/index", HTTP_GET, [this]() { handleIndex(); });
  server.on("/login", HTTP_GET, [this]() { handleLogin(); });
  server.on("/login", HTTP_POST, [this]() { handleLogin(); });
  server.onNotFound([this]() { handleNotFound(); });

    // Logo
  server.on("/img/logo.png", HTTP_GET, [this]() {
    size_t len = _binary_data_img_logo_png_end - _binary_data_img_logo_png_start;
    server.setContentLength(len);
    server.send(200, "image/png", "");
    server.sendContent_P(reinterpret_cast<const char*>(_binary_data_img_logo_png_start), len);
  });
  server.on("/setting", HTTP_GET, [this]() { handleSetting(); });
  // server.on("/setting_wifi", HTTP_GET, [this]() { handleSettingWifi(); }); // Removed - using modals in settings page
  server.on("/wifi_connect", HTTP_POST, [this]() { handleWifiConnect(); });
  
  // New API endpoints
  server.on("/api/device-name", HTTP_POST, [this]() { handleDeviceName(); });
  server.on("/api/reboot", HTTP_POST, [this]() { handleReboot(); });
  server.on("/api/factory-reset", HTTP_POST, [this]() { handleFactoryReset(); });
  server.on("/api/change-password", HTTP_POST, [this]() { handleChangePassword(); });
  server.on("/api/temp-unit", HTTP_POST, [this]() { handleTempUnit(); });
  server.on("/api/toggle-ap", HTTP_POST, [this]() { handleToggleAP(); });
  server.on("/api/mqtt-settings", HTTP_POST, [this]() { handleMQTTSettings(); });
  server.on("/api/mqtt-test", HTTP_POST, [this]() { handleMQTTTest(); });
  server.on("/api/mqtt-status", HTTP_GET, [this]() { handleMQTTStatus(); });
  server.on("/api/wifi-status", HTTP_GET, [this]() { handleWiFiStatus(); });
  server.on("/api/wifi-scan", HTTP_GET, [this]() { handleWiFiScan(); });
  server.on("/api/wifi-disconnect", HTTP_POST, [this]() { handleWiFiDisconnect(); });
  server.on("/api/wifi-clear", HTTP_POST, [this]() { handleWiFiClear(); });
  server.on("/api/led-toggle", HTTP_POST, [this]() { handleLEDToggle(); });
  server.on("/api/status", HTTP_GET, [this]() { handleStatus(); });
  server.on("/api/settings", HTTP_GET, [this]() { handleGetSettings(); });
}

void ServerManager::handleRoot() {
    if (!isAuthenticated()) {
        server.sendHeader("Location", "/login");
        server.send(302);
        return;
    }
    // Lade index.html als String
    String html(reinterpret_cast<const char*>(_binary_data_index_html_start),
                _binary_data_index_html_end - _binary_data_index_html_start);

    // Lade main.html als Content
    String pageContent(reinterpret_cast<const char*>(_binary_data_main_html_start),
                       _binary_data_main_html_end - _binary_data_main_html_start);

    html.replace("%CONTENT%", pageContent);
    html.replace("%DEVICE_NAME%", Config::DEVICE_NAME);
    html.replace("%TOKEN%", sessionToken);
    html.replace("%HOME_ACTIVE%", "active");
    html.replace("%SETTING_ACTIVE%", "");

    server.setContentLength(html.length());
    server.send(200, "text/html", html);
}

void ServerManager::handleLogin() {
  Serial.println("Login handler called");
  
  if (server.method() == HTTP_POST) {
    Serial.println("POST request received");
    String password = server.arg("password");
    Serial.print("Password received: ");
    Serial.println(password);
    
    if (password == Config::WEB_PASSWORD) {
      Serial.println("Password correct - redirecting");
      sessionToken = String(random(0xffff), HEX);
      
      // Direkte Weiterleitung zur Hauptseite
      server.sendHeader("Location", "/main?token=" + sessionToken);
      server.send(302);
      return;
    } else {
      Serial.println("Password incorrect");
    }
  } else {
    Serial.println("GET request - showing login page");
  }
  
  // Login-Seite anzeigen (bei GET oder falschem Passwort)
  Serial.println("Sending login page");
  size_t len = _binary_data_login_html_end - _binary_data_login_html_start;
  server.setContentLength(len);
  server.send(200, "text/html", "");
  server.sendContent_P(reinterpret_cast<const char*>(_binary_data_login_html_start), len);
}

void ServerManager::handleNotFound() {
    Serial.println("404 - Page not found: " + server.uri());
    server.send(404, "text/plain", "Nicht gefunden");
}

void ServerManager::handleMain() {
    if (!isAuthenticated()) {
        server.sendHeader("Location", "/login");
        server.send(302);
        return;
    }
    
    // Lade main.html direkt
    String html(reinterpret_cast<const char*>(_binary_data_main_html_start),
                _binary_data_main_html_end - _binary_data_main_html_start);

    // Replace dynamic values
    html.replace("%DEVICE_NAME%", Config::DEVICE_NAME);
    html.replace("%TOKEN%", sessionToken);

    server.setContentLength(html.length());
    server.send(200, "text/html", html);
}

void ServerManager::handleIndex() {
    if (!isAuthenticated()) {
        server.sendHeader("Location", "/login");
        server.send(302);
        return;
    }
    
    // Lade index.html direkt
    String html(reinterpret_cast<const char*>(_binary_data_index_html_start),
                _binary_data_index_html_end - _binary_data_index_html_start);

    // Replace dynamic values
    html.replace("%DEVICE_NAME%", Config::DEVICE_NAME);
    html.replace("%TOKEN%", sessionToken);

    server.setContentLength(html.length());
    server.send(200, "text/html", html);
}

bool ServerManager::isAuthenticated() {
    if (server.hasArg("token")) {
        return server.arg("token") == sessionToken;
    }
    return false;
}

void ServerManager::handleSetting() {
    if (!isAuthenticated()) {
        server.sendHeader("Location", "/login");
        server.send(302);
        return;
    }
    
    // Lade setting.html direkt
    extern const uint8_t _binary_data_setting_html_start[] asm("_binary_data_setting_html_start");
    extern const uint8_t _binary_data_setting_html_end[]   asm("_binary_data_setting_html_end");
    String html(reinterpret_cast<const char*>(_binary_data_setting_html_start),
                       _binary_data_setting_html_end - _binary_data_setting_html_start);

    // Replace dynamic values
    html.replace("%DEVICE_NAME%", Config::DEVICE_NAME);
    html.replace("%FIRMWARE_VERSION%", Config::FIRMWARE_VERSION);
    html.replace("%LAST_PASSWORD_CHANGE%", Config::getLastPasswordChange());
    
    // Temperature unit selection
    if (strcmp(Config::TEMP_UNIT, "fahrenheit") == 0) {
        html.replace("%CELSIUS_SELECTED%", "");
        html.replace("%FAHRENHEIT_SELECTED%", "selected");
    } else {
        html.replace("%CELSIUS_SELECTED%", "selected");
        html.replace("%FAHRENHEIT_SELECTED%", "");
    }
    
    // Access Point status
    String apStatus = Config::AP_ENABLED ? "Enabled" : "Disabled";
    String apButtonText = Config::AP_ENABLED ? "Disable" : "Enable";
    html.replace("%AP_STATUS%", apStatus);
    html.replace("%AP_BUTTON_TEXT%", apButtonText);
    
    // MQTT settings
    html.replace("%MQTT_SERVER%", Config::MQTT_SERVER);
    html.replace("%MQTT_PORT%", String(Config::MQTT_PORT));
    html.replace("%MQTT_USER%", Config::MQTT_USER);
    html.replace("%MQTT_PASS%", Config::MQTT_PASS);
    html.replace("%MQTT_TOPIC%", Config::MQTT_TOPIC);
    
    // Device ID for Home Assistant
    String deviceId = WiFi.macAddress().substring(9); // Last 3 bytes of MAC
    deviceId.replace(":", "");
    html.replace("%DEVICE_ID%", deviceId);

    // WiFi settings for modal
    String ssid, ip, mac, rssi, status, wifiOptions, savedPassword;
    
    if (WiFi.status() == WL_CONNECTED) {
        ssid = WiFi.SSID();
        ip = WiFi.localIP().toString();
        mac = WiFi.macAddress();
        rssi = String(WiFi.RSSI());
        status = "Connected. RSSI: " + rssi + "dBm";
    } else {
        ssid = "-";
        ip = "-";
        mac = WiFi.macAddress();
        rssi = "-";
        status = "Not connected";
    }
    
    // Load saved WiFi password
    Preferences prefs;
    if (prefs.begin("wifi", true)) {
        savedPassword = prefs.getString("password", "");
        prefs.end();
    }
    
    // Generate WiFi options
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i) {
        String networkSSID = WiFi.SSID(i);
        String selected = (networkSSID == WiFi.SSID()) ? " selected" : "";
        wifiOptions += "<option value=\"" + networkSSID + "\"" + selected + ">" + networkSSID + " (" + String(WiFi.RSSI(i)) + "dBm)</option>\n";
    }
    
    html.replace("%WIFI_SSID%", ssid);
    html.replace("%WIFI_IP%", ip);
    html.replace("%WIFI_MAC%", mac);
    html.replace("%WIFI_RSSI%", rssi);
    html.replace("%WIFI_STATUS%", status);
    html.replace("%WIFI_OPTIONS%", wifiOptions);
    html.replace("%WIFI_PASSWORD%", savedPassword);
    html.replace("%TOKEN%", sessionToken);
    html.replace("%HOME_ACTIVE%", "");
    html.replace("%SETTING_ACTIVE%", "active");

    server.setContentLength(html.length());
    server.send(200, "text/html", html);
}

/*
// DEPRECATED: WiFi configuration moved to modal dialogs in settings page
void ServerManager::handleSettingWifi() {
    if (!isAuthenticated()) {
        server.sendHeader("Location", "/login");
        server.send(302);
        return;
    }

    // Redirect to settings page with WiFi modal
    server.sendHeader("Location", "/setting");
    server.send(302);
}
*/

void ServerManager::handleWifiConnect() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    // Speichern in Preferences
    Preferences prefs;
    if (prefs.begin("wifi", false)) {
        prefs.putString("ssid", ssid);
        prefs.putString("password", password);
        prefs.end();
        Serial.println("WiFi credentials saved");
    } else {
        Serial.println("Error: Failed to save WiFi credentials");
        server.send(500, "text/plain", "Failed to save credentials");
        return;
    }

    wifi.beginSTA(ssid.c_str(), password.c_str());
    server.send(200, "text/plain", "OK");
}

void ServerManager::handleDeviceName() {
    if (!isAuthenticated()) {
        server.send(401, "application/json", "{\"success\":false,\"error\":\"Unauthorized\"}");
        return;
    }
    String deviceName = server.arg("name");
    if (deviceName.length() > 0 && deviceName.length() < 32) {
        Config::saveDeviceName(deviceName.c_str());
        server.send(200, "application/json", "{\"success\":true,\"message\":\"Device name saved\"}");
    } else {
        server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid device name\"}");
    }
}

void ServerManager::handleReboot() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    server.send(200, "text/plain", "OK");
    delay(1000);
    ESP.restart();
}

void ServerManager::handleFactoryReset() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    Config::factoryReset();
    server.send(200, "text/plain", "OK");
    delay(1000);
    ESP.restart();
}

void ServerManager::handleChangePassword() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    
    String currentPassword = server.arg("current_password");
    String newPassword = server.arg("new_password");
    
    // Validate current password
    if (currentPassword != Config::WEB_PASSWORD) {
        server.send(200, "text/plain", "INVALID");
        return;
    }
    
    // Validate new password
    if (newPassword.length() < 6 || newPassword.length() >= 32) {
        server.send(400, "text/plain", "Password must be 6-31 characters");
        return;
    }
    
    // Check if new password is different from current
    if (newPassword == currentPassword) {
        server.send(400, "text/plain", "New password must be different");
        return;
    }
    
    Config::saveWebPassword(newPassword.c_str());
    sessionToken = ""; // Invalidate current session
    server.send(200, "text/plain", "OK");
}

void ServerManager::handleTempUnit() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    String tempUnit = server.arg("unit");
    if (tempUnit == "celsius" || tempUnit == "fahrenheit") {
        Config::saveTempUnit(tempUnit.c_str());
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Invalid temperature unit");
    }
}

void ServerManager::handleToggleAP() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    String enable = server.arg("enable");
    bool apEnabled = (enable == "1");
    Config::saveAPEnabled(apEnabled);
    server.send(200, "text/plain", "OK");
    delay(1000);
    ESP.restart();
}

void ServerManager::handleMQTTSettings() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    String server_arg = server.arg("server");
    String port_arg = server.arg("port");
    String user_arg = server.arg("user");
    String pass_arg = server.arg("pass");
    String topic_arg = server.arg("topic");
    
    if (server_arg.length() > 0 && port_arg.toInt() > 0) {
        uint16_t port = port_arg.toInt();
        
        // Disconnect current MQTT connection
        mqttManager.disconnect();
        
        // Save new settings
        Config::saveMQTTSettings(server_arg.c_str(), port, user_arg.c_str(), pass_arg.c_str(), topic_arg.c_str());
        
        // Restart MQTT with new settings
        mqttManager.begin();
        
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Invalid MQTT settings");
    }
}

void ServerManager::handleMQTTTest() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    
    // Use current MQTT settings for test
    String testServer = String(Config::MQTT_SERVER);
    int testPort = Config::MQTT_PORT;
    String testUser = String(Config::MQTT_USER);
    String testPass = String(Config::MQTT_PASS);
    
    if (testServer.length() == 0 || testPort <= 0) {
        server.send(200, "application/json", "{\"success\":false,\"error\":\"No MQTT server configured\"}");
        return;
    }
    
    // Test MQTT connection
    WiFiClient testClient;
    PubSubClient testMqtt(testClient);
    
    testMqtt.setServer(testServer.c_str(), testPort);
    
    String clientId = "ESP32-Test-" + String(random(0xffff), HEX);
    bool connected;
    
    if (testUser.length() > 0) {
        connected = testMqtt.connect(clientId.c_str(), testUser.c_str(), testPass.c_str());
    } else {
        connected = testMqtt.connect(clientId.c_str());
    }
    
    if (connected) {
        testMqtt.disconnect();
        server.send(200, "application/json", "{\"success\":true}");
    } else {
        String error = "Connection failed (Code: " + String(testMqtt.state()) + ")";
        server.send(200, "application/json", "{\"success\":false,\"error\":\"" + error + "\"}");
    }
}

void ServerManager::handleWiFiStatus() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    
    String json = "{";
    if (WiFi.status() == WL_CONNECTED) {
        json += "\"connected\":true,";
        json += "\"ssid\":\"" + WiFi.SSID() + "\",";
        json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
        json += "\"mac\":\"" + WiFi.macAddress() + "\",";
        json += "\"status\":\"Connected (RSSI: " + String(WiFi.RSSI()) + "dBm)\"";
    } else {
        json += "\"connected\":false,";
        json += "\"ssid\":\"-\",";
        json += "\"ip\":\"-\",";
        json += "\"mac\":\"" + WiFi.macAddress() + "\",";
        json += "\"status\":\"Not connected\"";
    }
    json += "}";
    
    server.send(200, "application/json", json);
}

void ServerManager::handleWiFiScan() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    
    int n = WiFi.scanNetworks();
    String json = "{\"networks\":[";
    
    for (int i = 0; i < n; ++i) {
        if (i > 0) json += ",";
        json += "{";
        json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
        json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
        json += "\"channel\":" + String(WiFi.channel(i)) + ",";
        json += "\"encryption\":" + String(WiFi.encryptionType(i)) + ",";
        json += "\"selected\":" + String(WiFi.SSID(i) == WiFi.SSID() ? "true" : "false");
        json += "}";
    }
    
    json += "]}";
    server.send(200, "application/json", json);
}

void ServerManager::handleWiFiDisconnect() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    
    WiFi.disconnect();
    server.send(200, "text/plain", "OK");
}

void ServerManager::handleWiFiClear() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    
    // Disconnect from WiFi
    WiFi.disconnect();
    
    // Clear saved WiFi credentials from preferences
    Preferences prefs;
    if (prefs.begin("wifi", false)) {
        prefs.clear();
        prefs.end();
        Serial.println("WiFi credentials cleared from preferences");
        server.send(200, "text/plain", "OK");
    } else {
        Serial.println("Error: Failed to clear WiFi preferences");
        server.send(500, "text/plain", "Failed to clear credentials");
    }
}

void ServerManager::handleMQTTStatus() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    
    String json = "{";
    if (mqttManager.isConnected()) {
        json += "\"connected\":true,";
        json += "\"status\":\"Connected to " + String(Config::MQTT_SERVER) + "\"";
    } else {
        json += "\"connected\":false,";
        if (strlen(Config::MQTT_SERVER) == 0) {
            json += "\"status\":\"No MQTT server configured\"";
        } else {
            json += "\"status\":\"Not connected to " + String(Config::MQTT_SERVER) + "\"";
        }
    }
    json += "}";
    
    server.send(200, "application/json", json);
}

void ServerManager::handleLEDToggle() {
    if (!isAuthenticated()) {
        server.send(401, "text/plain", "Unauthorized");
        return;
    }
    
    static bool ledState = false;
    ledState = !ledState;
    
    if (ledState) {
        led.setColor(CRGB::Blue);  // LED an (blau)
    } else {
        led.setColor(CRGB::Black); // LED aus
    }
    
    server.send(200, "text/plain", "OK");
}

void ServerManager::handleStatus() {
    String status = "{";
    status += "\"uptime\":" + String(millis()/1000) + ",";
    status += "\"heap\":" + String(ESP.getFreeHeap()) + ",";
    status += "\"wifi_connected\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false") + ",";
    status += "\"wifi_ip\":\"" + WiFi.localIP().toString() + "\",";
    status += "\"wifi_rssi\":" + String(WiFi.RSSI()) + ",";
    status += "\"mqtt_connected\":" + String(mqttManager.isConnected() ? "true" : "false");
    status += "}";
    server.send(200, "application/json", status);
}

void ServerManager::handleGetSettings() {
    String settings = "{";
    settings += "\"device_name\":\"" + String(Config::DEVICE_NAME) + "\",";
    settings += "\"mqtt_server\":\"" + String(Config::MQTT_SERVER) + "\",";
    settings += "\"mqtt_port\":" + String(Config::MQTT_PORT) + ",";
    settings += "\"mqtt_user\":\"" + String(Config::MQTT_USER) + "\",";
    // Mask password for security - show asterisks if password exists
    String maskedPassword = "";
    if (strlen(Config::MQTT_PASS) > 0) {
        for (int i = 0; i < strlen(Config::MQTT_PASS); i++) {
            maskedPassword += "*";
        }
    }
    settings += "\"mqtt_pass\":\"" + maskedPassword + "\",";
    settings += "\"ap_enabled\":" + String(Config::AP_ENABLED ? "true" : "false") + ",";
    settings += "\"temp_unit\":\"" + String(Config::TEMP_UNIT) + "\",";
    settings += "\"mqtt_connected\":" + String(mqttManager.isConnected() ? "true" : "false");
    settings += "}";
    server.send(200, "application/json", settings);
}
