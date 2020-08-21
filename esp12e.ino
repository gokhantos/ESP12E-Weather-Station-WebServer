#include "ESP8266WiFi.h"
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include <Wire.h>
#include "SparkFunHTU21D.h"
#include <FS.h>
#define relayPin D6

const char* ssid = "TurkTelekom_Z9EM7";
const char* password ="1E5909eDc2257";

AsyncWebServer server(80);

HTU21D myHumidity;

String tempValue;
String humdValue;
String relayState;

String processor(const String& var){
  if(var == "STATE"){
    if(digitalRead(relayPin)){
      relayState = "ON";
    }
    else{
      relayState = "OFF";
    }
    return relayState;
  }
  return String();
}

void setup() {
  Serial.begin(115200);
  myHumidity.begin();
  pinMode(relayPin,OUTPUT);
//  digitalWrite(relayPin,LOW);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  SPIFFS.begin();
 
  Serial.println(WiFi.localIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/temperature.html", "text/html");
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/humidity.html", "text/html");
  });
  server.on("/readHumidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain",humdValue);
  });
  server.on("/readTemp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain",tempValue);
  });
  server.on("/relaymodule", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send(SPIFFS, "/relaymodule.html", String(), false, processor);
});
 server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
  digitalWrite(relayPin,HIGH);
  request->send(SPIFFS, "/relaymodule.html", String(), false, processor);
});
server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
  digitalWrite(relayPin,LOW);
  request->send(SPIFFS, "/relaymodule.html", String(), false, processor);
});
  
  server.begin();

}

void loop() {
float humidity=myHumidity.readHumidity();
humdValue=String(humidity);
float temp = myHumidity.readTemperature();
tempValue=String(temp);
Serial.println(temp);
}