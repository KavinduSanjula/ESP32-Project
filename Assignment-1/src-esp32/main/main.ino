#include <WiFi.h>
#include "time.h"
#include "DHT.h"

#define DHTTYPE DHT22

const char* ntpSever = "pool.ntp.org";

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

uint8_t DHTPin = 4;
DHT dht(DHTPin,DHTTYPE);

float temperature;
int lightLevel;

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.print("Connecting");
  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED){
    Serial.print('.');
    delay(500);
  }
  Serial.println("\nConnected.");

  configTime(gmtOffset_sec,daylightOffset_sec,ntpServer);
  
  pinMode(DHTPin,OUTPUT);
  dht.begin();

  delay(1000);
}

void loop() {
  temperature = dht.readTemperature();
  lightLevel = analogRead(A0);
}

void printValues(){
  struct tm timeinfo;
  
  if (!getLocalTime(&timeinfo))
    return;

  Serial.print(&timeinfo,"%H");
  Serial.print(',');
  Serial.print(&timeinfo,"%M");
  Serial.print(',');
  Serial.print(&timeinfo,"%S");
  Serial.print(',');
  Serial.print(temperature);
  Serial.print(',');
  Serial.println(lightLevel);
}
