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
const char* ssid = "{ssid}";
const char* password = "{pw}";

//Your Domain name with URL path or IP address with path
String serverName = "http://api.steampowered.com/ISteamUserStats/GetUserStatsForGame/v0002/?appid=730&key={key}&steamid={steam}";
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
  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());// Print the IP address
  for (int b=0; b<10; b++)
       {
    for (int a=0; a<6; a++)
      {
    lc.setDigit(0,a,b,false);

    }
      delay(250);
    }
    for (int b=0; b<10; b++)
       {
    for (int a=0; a<6; a++)
      {
    lc.setDigit(0,a,b,false);

    }
      delay(250);
    }
}
void loop()
{
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      http.begin(serverName.c_str());
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      String payload = http.getString();
      const char* conststring = payload.c_str();
      Serial.println(httpResponseCode);
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        const size_t capacity = JSON_ARRAY_SIZE(85) + JSON_ARRAY_SIZE(170) + JSON_OBJECT_SIZE(1) + 255*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 10150;
        DynamicJsonDocument doc(capacity);
        DeserializationError err = deserializeJson(doc, conststring);
        if (err) {
          Serial.print("Error: ");
          Serial.println(err.c_str());
          return;
        }
        JsonObject playerstats = doc["playerstats"];
        JsonArray playerstats_stats = playerstats["stats"];
        const char* playerstats_steamID = playerstats["steamID"];
        int kills = playerstats_stats[0]["value"];
        //Serial.println(payload);
        //Serial.println("Help");
         Serial.println((kills%10));
         Serial.println((kills%100)/10);
         Serial.println((kills%1000)/100);
         Serial.println((kills%10000)/1000);
         Serial.println((kills%100000)/10000);
         lc.setDigit(0,0,((kills%1000000)/100000),false);
         lc.setDigit(0,1,((kills%100000)/10000),false);
         lc.setDigit(0,2,((kills%10000)/1000),false);
         lc.setDigit(0,3,((kills%1000)/100),false);
         lc.setDigit(0,4,((kills%100)/10),false);
         lc.setDigit(0,7,((kills%10)),false);

        
                              delay(600000);
      http.end();
      } 
    }
    else {
      Serial.println("WiFi Disconnected");
    }
     
    lastTime = millis();
  }
  }
