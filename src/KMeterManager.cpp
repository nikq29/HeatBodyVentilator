#include "KMeterManager.h"

KMeterManager::KMeterManager() {
    initialized = false;
    lastReadTime = 0;
    currentTempCelsius = 0.0;
    currentTempFahrenheit = 0.0;
    internalTempCelsius = 0.0;
    errorStatus = 255; // Uninitialized
    
    // Standard-Konfiguration
    i2cAddress = 0x66;
    sdaPin = 21;
    sclPin = 22;
    i2cSpeed = 100000L;
    readInterval = 1000; // 1 Sekunde
}

bool KMeterManager::begin(uint8_t addr, uint8_t sda, uint8_t scl, uint32_t speed) {
    Serial.println("DEBUG: KMeterManager::begin() called");
    
    i2cAddress = addr;
    sdaPin = sda;
    sclPin = scl;
    i2cSpeed = speed;
    
    // Initialisiere KMeter-ISO Sensor
    if (kmeter.begin(&Wire, i2cAddress, sdaPin, sclPin, i2cSpeed)) {
        initialized = true;
        Serial.printf("KMeter-ISO initialized on I2C address 0x%02X\n", i2cAddress);
        
        // Erste Messung
        update();
        return true;
    } else {
        initialized = false;
        Serial.printf("Failed to initialize KMeter-ISO on I2C address 0x%02X\n", i2cAddress);
        return false;
    }
}

void KMeterManager::update() {
    if (!initialized) return;
    
    unsigned long currentTime = millis();
    if (currentTime - lastReadTime < readInterval) {
        return; // Noch nicht Zeit für nächste Messung
    }
    
    // Lese Sensor-Status
    errorStatus = kmeter.getReadyStatus();
    
    if (errorStatus == 0) {
        // Sensor ist bereit, lese Temperaturen
        int32_t tempC = kmeter.getCelsiusTempValue();
        int32_t tempF = kmeter.getFahrenheitTempValue();
        int32_t internalC = kmeter.getInternalCelsiusTempValue();
        
        // Konvertiere von int32_t (Wert * 100) zu float
        currentTempCelsius = (float)tempC / 100.0;
        currentTempFahrenheit = (float)tempF / 100.0;
        internalTempCelsius = (float)internalC / 100.0;
        
        Serial.printf("KMeter-ISO: %.2f°C / %.2f°F (Internal: %.2f°C)\n", 
                     currentTempCelsius, currentTempFahrenheit, internalTempCelsius);
    } else {
        Serial.printf("KMeter-ISO Error Status: %d\n", errorStatus);
    }
    
    lastReadTime = currentTime;
}

String KMeterManager::getStatusString() const {
    if (!initialized) return "Not Initialized";
    
    switch (errorStatus) {
        case 0: return "Ready";
        case 1: return "Sensor Error";
        case 2: return "Communication Error";
        case 3: return "Data Not Ready";
        default: return "Unknown Error (" + String(errorStatus) + ")";
    }
}

bool KMeterManager::setI2CAddress(uint8_t addr) {
    if (!initialized) return false;
    
    if (kmeter.setI2CAddress(addr)) {
        i2cAddress = addr;
        Serial.printf("KMeter-ISO I2C address changed to 0x%02X\n", addr);
        return true;
    } else {
        Serial.printf("Failed to change KMeter-ISO I2C address to 0x%02X\n", addr);
        return false;
    }
}

uint8_t KMeterManager::getFirmwareVersion() {
    if (!initialized) return 0;
    return kmeter.getFirmwareVersion();
}
