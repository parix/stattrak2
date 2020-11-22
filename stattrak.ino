#include "LedControl.h" //  need the library
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
LedControl lc=LedControl(5,4,0,1); // lc is our object
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219
const char* ssid = "router";
const char* password = "password";

//Your Domain name with URL path or IP address with path
String serverName = "http://api.steampowered.com/ISteamUserStats/GetUserStatsForGame/v0002/?appid=730&key=XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX&steamid=XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;


void setup()
{
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  Serial.begin(115200);
  lc.shutdown(0,false);// turn off power saving, enables display
  lc.setIntensity(0,8);// sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
    Serial.begin(115200); 

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}
void loop()
{
    for (int b=0; b<9; b++)
       {
    for (int a=0; a<6; a++)
      {
    lc.setDigit(0,a,b,false);

    }
      delay(250);
    }

  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      String serverPath = serverName;
      http.begin(serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      String payload = http.getString();
      const char* conststring = payload.c_str();
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        const size_t capacity = JSON_ARRAY_SIZE(164) + JSON_ARRAY_SIZE(212) + JSON_OBJECT_SIZE(1) + 376*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 8620;
        DynamicJsonDocument doc(capacity);
        DeserializationError err = deserializeJson(doc, conststring);
        if (err) {
          Serial.print("Error: ");
          Serial.println(err.c_str());
          return;
        }
        JsonObject playerstats = doc["playerstats"];
        const char* playerstats_steamID = playerstats["steamID"];
        Serial.println(playerstats_steamID);      
        }
      http.end();
    }
    }
    else {
      Serial.println("WiFi Disconnected");
    } 
    lastTime = millis();
  }
