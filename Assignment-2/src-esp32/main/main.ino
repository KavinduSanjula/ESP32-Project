#include <WiFi.h>
#include <HTTPClient.h>
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

  postData();
  dealy(10000);
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

String getTimeStamp(){
  struct tm timeinfo;
  
  if (!getLocalTime(&timeinfo))
    return;

  char _year[2];
  strftime(_year,2,"%Y",&timeinfo);
  char _mon[2];
  strftime(_mon,2,"%B",&timeinfo);
  char _day[2];
  strftime(_day,2,"%d",&timeinfo);

  char _hour[2];
  strftime(_hour,2,"%H",&timeinfo);
  char _min[2];
  strftime(_min,2,"%H",&timeinfo);
  char _sec[2];
  strftime(_sec,2,"%H",&timeinfo);

  return String(_year+_mon+_day+_hour+_min+_sec);
}

void postData(){
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://192.168.43.236/esp-32-api/esp-api.php"); //HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = http.POST("{\"timestamp\":\"" + getTimestamp() +"\",\"temp\":\"" + String(temperature)"\",\"light:\""+ String(lightLevel)+"\"}");

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}
