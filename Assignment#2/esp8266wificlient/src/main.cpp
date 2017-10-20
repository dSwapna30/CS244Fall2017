#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "secret.h"
#include "MAX30105.h"

// Create "secret.h" for "ssid" and "host"

HTTPClient http;
MAX30105 max;
    

void sensorSetup(byte powerLevel){

      byte sampleAverage = 4;
      byte ledMode = 2;
      int sampleRate = 50;
      int pulseWidth = 411; //Options: 69, 118, 215, 411
      int adcRange = 2048;

    max.setup(powerLevel, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void sendtoWebServer(int sample){
    uint32_t IR;
    uint32_t RED;

    max.check();
    while(max.available()){
        IR=getir();
        RED=getred();
    }
    
    
    host = host + "?sample=" + sample + "&IR=" + IR + "&RED" + RED;
    http.begin(host.c_str());
    int httpCode = http.GET();

    if (httpCode > 0) {
        Serial.print("POST code: ");
        Serial.println(httpCode);
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(payload);
        }
    } else {
        Serial.print("POST failed, error: ");
        Serial.println(http.errorToString(httpCode).c_str());
    }

    http.end();

}

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println();

    // Show MAC Address
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    Serial.println();

    // Connect to WiFi and show IP Address
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println();
    Serial.println("closing connection");
}


int counter = 0;
void loop() {
    if (counter == 0) {sensorSetup(0x02)};
    if (counter == 6000) {sensorSetup(0x1F)};
    if (counter == 12000) {sensorSetup(0x7F)};
    if (counter == 18000) {sensorSetup(0xFF)};
    sendtoWebServer(counter);
    counter++;
}

