#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "secret.h"

// Create "secret.h" for "ssid" and "host"

HTTPClient http;
    
int t = 1; // time
int data = 1;

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

void loop() {
    t++;
    data = data * 2;
    host = host + "?time=" + t + "&data=" + data;
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

    delay(1000);
}

