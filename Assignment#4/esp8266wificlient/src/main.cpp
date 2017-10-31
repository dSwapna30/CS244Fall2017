#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "secret.h"
#include "MAX30105.h"
#include "Wire.h"

int counter = 0;
int loop_sample = 0;

// Create "secret.h" for "ssid" and "host"

HTTPClient http;
MAX30105 MAX;

void sensorSetup(byte powerLevel){
      Serial.print("setup sensor to: ");
      Serial.println(String(powerLevel));
      Serial.println();

      byte sampleAverage = 4;
      byte ledMode = 2;
      int sampleRate = 50;
      int pulseWidth = 411; //Options: 69, 118, 215, 411
      int adcRange = 2048;

      MAX.setup(powerLevel, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void sendtoWebServer(int sample){
    uint32_t IR;
    uint32_t Red;

    MAX.check();
    if(MAX.available()){
        IR = MAX.getIR();
        Red = MAX.getRed();
    }
    
    
    String host = base_host + "?sample=" + sample + "&IR=" + IR + "&RED=" + Red;
    http.begin(host.c_str());
    int httpCode = http.GET();

    //if (httpCode > 0) {
    //    Serial.print("POST code: ");
    //    Serial.println(httpCode);
    //    if (httpCode == HTTP_CODE_OK) {
    //        Serial.print(host);
    //        Serial.println(" succeeded");
    //    }
    //} else {
    //    Serial.print("POST failed, error: ");
    //    Serial.println(http.errorToString(httpCode).c_str());
    //}

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
    //WiFi.begin(ssid, password);
    WiFi.begin(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    if (!MAX.begin(Wire, I2C_SPEED_STANDARD)) //Use default I2C port, 100kHz speed
    {
      Serial.println("MAX30105 was not found. Please check wiring/power.");
      while (1);
    }

    Serial.println();
}

void loop() {
    if (counter == 0) {
      loop_sample = 0;
      sensorSetup(0xFF);
    }
    //if (counter == 6000) {
    //  loop_sample = 0;
    //  sensorSetup(0x1F);
    //}
    //if (counter == 12000) {
    //  loop_sample = 0;
    //  sensorSetup(0x7F);
    //}
    //if (counter == 18000) {
    //  loop_sample = 0;
    //  sensorSetup(0xFF);
    //}
    if (counter < 6000) {
      sendtoWebServer(loop_sample);
    }
    //Serial.println(counter);
    loop_sample = loop_sample + 1;
    counter = counter + 1;
}
