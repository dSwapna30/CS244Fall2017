#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
//#include "secret.h"
#include "MAX30105.h"
#include "Wire.h"
#include "SparkFunLIS3DH.h"
#include "SPI.h"

const char* ssid = "UCInet Mobile Access";
String base_host = "http://169.234.241.150/arduino_simple.php";

int counter = 0;
int loop_counter = 0;
const int send_counter = 100;
const int time_limit = 600000;

unsigned long start_time = 0;
unsigned long interval_time = 0;

String body_data = "data=";

// Create "secret.h" for "ssid" and "host"

HTTPClient http;
MAX30105 MAX;
LIS3DH myIMU(I2C_MODE, 0x19); // constructed with parameters for I2C

void sensorSetup(byte powerLevel){
      Serial.print("setup sensor to: ");
      Serial.println(String(powerLevel));
      Serial.println();

      byte sampleAverage = 4;
      byte ledMode = 2;
      int sampleRate = 50;
      int pulseWidth = 411; //Options: 69, 118, 215, 411
      int adcRange = 4096;

      MAX.setup(powerLevel, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void sendtoWebServer(int c){

    Serial.println(body_data);
    
    http.begin(base_host.c_str());
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(body_data);
    body_data = "data=";
    
    if (httpCode > 0) {
        Serial.print("POST code: ");
        Serial.println(httpCode);
        if (httpCode == HTTP_CODE_OK) {
            Serial.print(base_host);
            Serial.println(" succeeded");
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
    WiFi.begin(ssid, password);
    //WiFi.begin(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Accelerometer setup
    myIMU.settings.accelSampleRate = 50;  //Hz.  Can be: 0,1,10,25,50,100,200,400,1600,5000 Hz
    myIMU.settings.accelRange = 2;      //Max G force readable.  Can be: 2, 4, 8, 16
    myIMU.settings.xAccelEnabled = 1;
    myIMU.settings.yAccelEnabled = 1;
    myIMU.settings.zAccelEnabled = 1;
    //Call .begin() to configure the Accelerometer
    //myIMU.begin();

    // PPG sensor setup
    if (!MAX.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 100kHz speed
    {
      Serial.println("MAX30105 was not found. Please check wiring/power.");
      while (1);
    }
    sensorSetup(0x1F); // 0x02, 0x1F, 0x7F, 0xFF
    
    //Call .begin() to configure the Accelerometer
    myIMU.begin();

    Serial.println();

    // Indicate that all setup has finished and ready to get data
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    start_time = millis();
}

void loop() {
    interval_time = millis();
    if ((millis() - start_time) < time_limit) {
      body_data = body_data + String(loop_counter) + "," + String(float(MAX.getIR())) + "," + String(float(MAX.getRed())) + "," + String(myIMU.readFloatAccelX()) + "," + String(myIMU.readFloatAccelY()) + "," + String(myIMU.readFloatAccelZ()) + "\n\r";
      counter = counter + 1;
      if ((counter % send_counter) == 0) {
        Serial.println(millis() - start_time);
        sendtoWebServer(loop_counter - send_counter);
        counter = 0;
      }
      Serial.println(millis() - interval_time);
      loop_counter = loop_counter + 1;
    }
    if ((millis() - interval_time) <= 20) {
      delay(20 - (millis() - interval_time));
    }
}