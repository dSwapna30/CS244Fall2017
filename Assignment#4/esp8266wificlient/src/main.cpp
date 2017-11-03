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
const int counter_limit = 3000;
const int send_counter = 100;

float x[send_counter+1];
float y[send_counter+1];
float z[send_counter+1];

unsigned long start_time = 0;

// Create "secret.h" for "ssid" and "host"

HTTPClient http;
//MAX30105 MAX;
LIS3DH myIMU(SPI_MODE, 5); // constructed with parameters for SPI and cs pin number

void sendtoWebServer(int c){

    String body_data = "data=";
    for (int i = 0; i < send_counter; i++) {
      body_data = body_data + String(c + i) + "," + String(x[i]) + "," + String(y[i]) + "," + String(z[i]) + "\n\r";
    }
    //Serial.println(body_data);
    //Serial.println(counter);
    
    http.begin(base_host.c_str());
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(body_data);

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

    // Accelerometer settings 
    myIMU.settings.accelSampleRate = 100;  //Hz.  Can be: 0,1,10,25,50,100,200,400,1600,5000 Hz
    myIMU.settings.accelRange = 2;      //Max G force readable.  Can be: 2, 4, 8, 16
    myIMU.settings.xAccelEnabled = 1;
    myIMU.settings.yAccelEnabled = 1;
    myIMU.settings.zAccelEnabled = 1;
    //Call .begin() to configure the Accelerometer
    myIMU.begin();

    Serial.println();

    start_time = millis();
}

void loop() {   
    if (loop_counter < counter_limit) {
      x[counter] = myIMU.readFloatAccelX();
      y[counter] = myIMU.readFloatAccelY();
      z[counter] = myIMU.readFloatAccelZ();
      Serial.println(x[counter]);
      counter = counter + 1;
      //if ( counter == send_counter) {
      if ((counter % send_counter) == 0) {
        Serial.println(millis() - start_time);
        sendtoWebServer(loop_counter - send_counter);
        //Serial.println("reached");
        counter = 0;
      }
    }
    /*if ( counter == send_counter) {
        sendtoWebServer();
    }*/
    //Serial.println(counter);
    loop_counter = loop_counter + 1;
}
