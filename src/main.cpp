#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <AHT10.h>
#include <BH1750.h>
#include "Timer.h"
#include "env.h"

Adafruit_BMP280 bmp;
AHT10 myAHT20(AHT10_ADDRESS_0X38, AHT20_SENSOR);
BH1750 lightMeter(0x23);

Timer timerPressure(600000);  //замер освещённости
Timer timerTemper(6000);  

// const char* ssid = "";
// const char* password = "";
const char* mqtt_server = "192.168.1.34";

WiFiClient espClient;
PubSubClient client(espClient);

const char* msgTemper="balcony/temper";
const char* msgDHTemper="balcony/temperDHT";
const char* msgPressure="balcony/pressure";
const char* msgHumidity="balcony/humidity";
const char* msgLight="balcony/light";
String pressure{};
//-----------------------------------
void setup_wifi() {

  delay(10);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("WiFi connected");
  randomSeed(micros());
  delay(1000);
}
//-----------------------------------
void reconnect() {
  // Loop until we're reconnected
  if (!client.connected()) {
    while (!client.connected()) {
      String clientId = "Balcony";
      // clientId += String(random(0xffff), HEX);
      // Attempt to connect
      if (!client.connect(clientId.c_str())) {
        // client.subscribe(msgPressure, 0);
        // client.subscribe("aisle/ledBlinkG");
      // } else {
        delay(1000);
      }
    }
  }
}
//-----------------------------------
void setup_280(){
  Serial.println(F("BMP280 Forced Mode Test."));

  //if (!bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X16,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_4000); /* Standby time. */
}
//---------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println(F("AHT20+BMP280 test"));
  Wire.begin(); //SDA, SCL);
  Serial.println(F("Wire.begin"));
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  reconnect();
  setup_280();

  while (myAHT20.begin() != true) {
    Serial.println(F("AHT20 not connected or fail to load calibration coefficient")); //(F()) save string to flash & keeps dynamic memory free
    delay(5000);
  }
  Serial.println(F("AHT20 OK"));
  setup_280();

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }
}
//----------------------------------------
void controlWiFi(){
  if(WiFi.status() != WL_CONNECTED) {
    delay(10000);
    ESP.restart();
  }
}
//---------------------------------------------------------
void loop() {
  controlWiFi();  //проверка подключения wifi
  reconnect();    //проверка подключения к mqtt
  client.loop();

  if(timerTemper.getTimer()){
    timerTemper.setTimer();
    if (bmp.takeForcedMeasurement()) {
      // double temp = myAHT20.readTemperature();
      // client.publish( msgTemper, String(temp).c_str() );
      if(timerPressure.getTimer()){
        timerPressure.setTimer();
          double pressure = bmp.readPressure() * 0.00750062;
          client.publish( msgPressure, String(pressure).c_str() );
      }
    }
    client.publish(msgTemper, String(myAHT20.readTemperature()).c_str());
    client.publish(msgHumidity, String(myAHT20.readHumidity()).c_str());

      // Serial.printf("Temper AHT: %.03f *C\n", myAHT20.readTemperature());
      // Serial.printf("Temper BMP: %.03f *C\n", bmp.readTemperature());
      // Serial.printf("Humidity: %.02f %RH\n", myAHT20.readHumidity());
      // Serial.printf("Pressure: %.02f hPa\n", bmp.readPressure() * 0.00750062);
    if (lightMeter.measurementReady()) {
      float lux = lightMeter.readLightLevel();
      client.publish(msgLight, String(lux).c_str());
    }
  }
}