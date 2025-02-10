/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com 

  upgrade and development Mariusz Katerla
*********/

// Import required libraries
#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
#endif
#include <OneWire.h>
#include <DallasTemperature.h>
#include "htmlPage.h"

// Data wire is connected to GPIO 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// Define numbers of pins to control valves
#define VALVE_UP_OPEN_PIN 13
#define VALVE_UP_CLOSE_PIN 16
#define VALVE_DOWN_OPEN_PIN 17
#define VALVE_DOWN_CLOSE_PIN 18

//Address of temp sensors
DeviceAddress termPZ = {0x28, 0x49, 0x51, 0xA4, 0x22, 0x22, 0x06, 0x54};
DeviceAddress termPP = {0x28, 0xA7, 0x78, 0x4A, 0x61, 0x22, 0x06, 0xEC};
DeviceAddress termParZ = {0x28, 0x0B, 0x19, 0x95, 0x61, 0x22, 0x06, 0x8A};
DeviceAddress termParP = {0x28, 0xC4, 0xDA, 0xA5, 0x61, 0x22, 0x06, 0x15};

// Variables to store temperature values
String temperatureCPz = "";
String temperatureCPp = "";
String temperatureCParZ = "";
String temperatureCParP = "";

String setTempUp = "";
String setTempDown = "";

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 5000;
unsigned long lastTimeSteerValve = 0;  
unsigned long timerDelayValve = 10000;
unsigned long lastTimeDurationValve = 0;  
unsigned long timerDurationValve = 2000; //how long the valve is opening

// Replace with your network credentials
const char* ssid = "Router";
const char* password = "1234";

const char* PARAM_TEMP_UP = "tempUp";
const char* PARAM_TEMP_DOWN = "tempDown";

bool switchOffMarkerUp = false;
bool switchOffMarkerDown = false;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readDSTemperatureC(const uint8_t *addr) {
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
  float tempC = sensors.getTempC(addr);

  if(tempC == -127.00) {
    Serial.println("Failed to read from DS18B20 sensor");
    return "--";
  } else {
    Serial.print("Temperature Celsius: ");
    Serial.println(tempC); 
  }
  return String(tempC);
}

// Replaces placeholder with DS18B20 values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURECPZ"){
    return temperatureCPz;
  }
  else if(var == "TEMPERATURECPP"){
    return temperatureCPp;
  }
  else if(var == "TEMPERATURECPARZ"){
    return temperatureCParZ;
  }
  else if(var == "TEMPERATURECPARP"){
    return temperatureCParP;
  }
  else if(var == "SET_TEMP_UP") {
    return setTempUp;
  }
  else if(var == "SET_TEMP_DOWN") {
    return setTempDown;
  }
  
  return String();
}

void WiFiDisconnect(WiFiEvent_t event, WiFiEventInfo_t info){
  WiFi.begin(ssid, password);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();
  
  // Start up the DS18B20 library
  sensors.begin();

  temperatureCPz = readDSTemperatureC(termPZ);
  temperatureCPp = readDSTemperatureC(termPP);
  temperatureCParZ = readDSTemperatureC(termParZ);
  temperatureCParP = readDSTemperatureC(termParP);

  setTempUp = "25.0";
  setTempDown = "25.0";

  WiFi.disconnect(true); //delete old config
  delay(1000);
  WiFi.onEvent(WiFiDisconnect, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperaturecPz", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatureCPz.c_str());
  });
  server.on("/temperaturecPp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatureCPp.c_str());
  });
  server.on("/temperaturecParZ", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatureCParZ.c_str());
  });
  server.on("/temperaturecParP", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatureCParP.c_str());
  });
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_TEMP_UP) && request->hasParam(PARAM_TEMP_DOWN)) {
      setTempUp = request->getParam(PARAM_TEMP_UP)->value();
      setTempDown = request->getParam(PARAM_TEMP_DOWN)->value();
    }
    else {
      setTempUp = "25.0";
      setTempDown = "25.0";
    }
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
}
 
void loop(){

  if ((millis() - lastTime) > timerDelay) {
    temperatureCPz = readDSTemperatureC(termPZ);
    temperatureCPp = readDSTemperatureC(termPP);
    temperatureCParZ = readDSTemperatureC(termParZ);
    temperatureCParP = readDSTemperatureC(termParP);
    lastTime = millis();
  } 

  if ((millis() - lastTimeSteerValve) > timerDelayValve) {

    if ((setTempUp.toFloat()-temperatureCPz.toFloat()) >= 1.0) {
      digitalWrite(VALVE_UP_CLOSE_PIN, LOW);
      digitalWrite(VALVE_UP_OPEN_PIN, HIGH);
      switchOffMarkerUp = true;
      Serial.println("Rozpoczecie grzania gora");
      Serial.println(setTempUp.toFloat());
    }
    else if ((setTempUp.toFloat()-temperatureCPz.toFloat()) <= -1.0) {
      digitalWrite(VALVE_UP_OPEN_PIN, LOW);
      digitalWrite(VALVE_UP_CLOSE_PIN, HIGH);
      switchOffMarkerUp = true;
    }
    else {
      digitalWrite(VALVE_UP_OPEN_PIN, LOW);
      digitalWrite(VALVE_UP_CLOSE_PIN, LOW);
    }

    if ((setTempDown.toFloat()-temperatureCParZ.toFloat()) >= 1.0) {
      digitalWrite(VALVE_DOWN_CLOSE_PIN, LOW);
      digitalWrite(VALVE_DOWN_OPEN_PIN, HIGH);
      switchOffMarkerDown = true;
      Serial.println("Rozpoczecie grzania dol");
      Serial.println(setTempDown.toFloat());
    }
    else if ((setTempDown.toFloat()-temperatureCParZ.toFloat()) <= -1.0) {
      digitalWrite(VALVE_DOWN_OPEN_PIN, LOW);
      digitalWrite(VALVE_DOWN_CLOSE_PIN, HIGH);
      switchOffMarkerDown = true;
    }
    else {
      digitalWrite(VALVE_DOWN_CLOSE_PIN, LOW);
      digitalWrite(VALVE_DOWN_OPEN_PIN, LOW);
    }

    lastTimeSteerValve = millis();
    lastTimeDurationValve = millis();
  }

  if ((millis() - lastTimeDurationValve) > timerDurationValve) {
    
    if (switchOffMarkerUp == true) {
      digitalWrite(VALVE_UP_OPEN_PIN, LOW);
      digitalWrite(VALVE_UP_CLOSE_PIN, LOW);
      switchOffMarkerUp = false;
      Serial.println("Gora grzanie");
    }

    if (switchOffMarkerDown == true) {
      digitalWrite(VALVE_DOWN_CLOSE_PIN, LOW);
      digitalWrite(VALVE_DOWN_OPEN_PIN, LOW);
      switchOffMarkerDown = false;
      Serial.println("Dol grzanie");
    }
    
  }

}