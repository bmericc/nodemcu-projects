#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "kalehost";
const char* password = "62davsan";

// Pin use
#define ONEWIRE 5 //pin to use for One Wire interface

// Set up which Arduino pin will be used for the 1-wire interface to the sensor
OneWire oneWire(ONEWIRE);
DallasTemperature sensors(&oneWire);

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "application/json", "{\"message\":\"Temperature monitor\"}");
}

void handleTemperatureResult() {  
  static float tempF;
  static float tempC;

  Serial.println();
  Serial.println("Requesting temperature...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  tempC = sensors.getTempCByIndex(0);
  tempF = DallasTemperature::toFahrenheit(tempC);

  server.send(200, "application/json", "{\"temperature\":"+String(tempC)+", \"fahrenheit\":"+String(tempF)+", \"humidity\":"+String(tempC)+"}");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  sensors.begin();
 
  Serial.println("");  

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
 

  server.on("/", handleRoot);
  server.on("/temperature", handleTemperatureResult); 

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
