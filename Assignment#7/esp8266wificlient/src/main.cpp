#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
//#include "secret.h"
#include "MAX30105.h"
#include "Wire.h"
#include "SparkFunLIS3DH.h"
#include "SPI.h"
#include "FS.h"

#define TESTFILE "/data.txt"

int counter = 0;
const int send_counter = 300;
const int counter_limit = 30000;

const int httpPort = 80;

int Activity = 1;

bool httpPostWorks = false;
bool spiffsActive = false;

unsigned long start_time = 0;

// Create "secret.h" for "ssid" and "host"

HTTPClient http;
MAX30105 MAX;
LIS3DH myIMU(I2C_MODE, 0x19); // constructed with parameters for I2C
WiFiClient client;

void sensorSetup(byte powerLevel) {

  byte sampleAverage = 1;
  byte ledMode = 2;

  MAX.setup(powerLevel, sampleAverage, ledMode, 50, 411, 4096);
}

void sendtoWebServer() {

  Serial.println(millis() - start_time);

  if (!client.connect(host, httpPort)) {
    Serial.println("Failed to connect HTTP POST!");
  }
  else {

    File f = SPIFFS.open(TESTFILE, "r");
    if (!f) {
      Serial.print("Unable To Open '");
      Serial.print(TESTFILE);
      Serial.println("' for Reading");
      Serial.println();
    } else {
      int len = 0;
      f = SPIFFS.open(TESTFILE, "r");
      while (f.position() < f.size())
      {
        String s = f.readStringUntil('\n');
        len = len + s.length();
      }
      String d = "data=";
      len = len + d.length() + 1;
      f.close();

      String request = String("POST ") + url +  " HTTP/1.1\r\n" +
                       "Host: " + host + "\r\n" +
                       "Content-Type: application/x-www-form-urlencoded\r\n" +
                       "Content-Length: " + String(len) + "\r\n" +
                       "Connection: open\r\n";
      client.print(request);
      client.print("\n data=");

      f = SPIFFS.open(TESTFILE, "r");
      while (f.position() < f.size())
      {
        String s = f.readStringUntil('\n');
        client.print(s);
      }
      client.println();
      f.close();
    }

    f = SPIFFS.open(TESTFILE, "w+");
    f.close();
  }
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

  // Test whether HTTP POST works
  http.begin(base_host.c_str());
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST("data=Time,IR,RED,Activity,X,Y,Z\n");

  if (httpCode > 0) {
    Serial.print("POST code: ");
    Serial.println(httpCode);
    if (httpCode == HTTP_CODE_OK) {
      Serial.print(base_host);
      Serial.println(" succeeded");
      httpPostWorks = true;
    }
  } else {
    Serial.print("POST failed! Check web server. Error: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }

  // Accelerometer setup
  myIMU.settings.accelSampleRate = 50;  //Hz.  Can be: 0,1,10,25,50,100,200,400,1600,5000 Hz
  myIMU.settings.accelRange = 2;      //Max G force readable.  Can be: 2, 4, 8, 16
  myIMU.settings.xAccelEnabled = 1;
  myIMU.settings.yAccelEnabled = 1;
  myIMU.settings.zAccelEnabled = 1;
  //Call .begin() to configure the Accelerometer
  myIMU.begin();

  // Start filing subsystem
  if (SPIFFS.begin()) {
    Serial.println("SPIFFS Active");
    Serial.println();
    spiffsActive = true;
  } else {
    Serial.println("Unable to activate SPIFFS");
  }
  File f = SPIFFS.open(TESTFILE, "w+");
  //f.print("data=");
  f.close();

  // PPG sensor setup
  if (!MAX.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 100kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1);
  }
  sensorSetup(0x1F); // 0x02, 0x1F, 0x7F, 0xFF

  // Indicate that all setup has finished and ready to get data
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  start_time = millis();

}

void loop() {
  if (httpPostWorks && counter < counter_limit) {
    MAX.check(); //Check the sensor, read up to 3 samples
    while (MAX.available()) //do we have new data?
    {
      counter++;

      if (spiffsActive) {
        if (SPIFFS.exists(TESTFILE)) {
          File f = SPIFFS.open(TESTFILE, "a");
          if (!f) {
            Serial.print("Unable To Open '");
            Serial.print(TESTFILE);
            Serial.println("' for Appending");
            Serial.println();
          } else {
            String data = (String)counter + "," +
                          (String)MAX.getFIFOIR()  + "," +
                          (String)MAX.getFIFORed() + "," +
                          (String)Activity + "," +
                          (String)myIMU.readFloatAccelX() + "," +
                          (String)myIMU.readFloatAccelY() + "," +
                          (String)myIMU.readFloatAccelZ() + "|\n";
            f.print(data);
            if ((counter % send_counter) == 0) {
              f.close();
              sendtoWebServer();
            }
          }
        }
      }
      MAX.nextSample(); //We're finished with this sample so move to next sample
    }
  }
}
