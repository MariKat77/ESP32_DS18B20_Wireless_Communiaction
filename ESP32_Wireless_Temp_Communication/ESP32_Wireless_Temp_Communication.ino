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

// Data wire is connected to GPIO 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

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

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 5000;

// Replace with your network credentials
const char* ssid = "Router";
const char* password = "1234";

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


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html lang="pl">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="utf-8">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    body { 
    background-image: url('https://www.pixelstalk.net/wp-content/uploads/images6/The-best-Minimalist-Desktop-Wallpaper-HD.jpg'); 
    background-repeat: repeat;
    background-size: cover; 
    }
    h2 { font-size: 3.0rem; }
    h3 { font-size: 2.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .ds-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Temperatury zasilania i powrotów z podłogówki</h2>
  <h3>Temperatury piętro</h3>
  <p>
    <i class="fas fa-thermometer-half" style="color:#f21d1d;"></i>
    <span class="ds-labels">Temperatura zasilania </span>
    <span id="temperaturecPz">%TEMPERATURECPZ%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-thermometer-half" style="color:#1565c0;"></i> 
    <span class="ds-labels">Temperatura powrotu </span>
    <span id="temperaturecPp">%TEMPERATURECPP%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <h3>Temperatury parter</h3>
  <p>
    <i class="fas fa-thermometer-half" style="color:#f21d1d;"></i>
    <span class="ds-labels">Temperatura zasilania </span>
    <span id="temperaturecParZ">%TEMPERATURECPARZ%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-thermometer-half" style="color:#1565c0;"></i> 
    <span class="ds-labels">Temperatura powrotu </span>
    <span id="temperaturecParP">%TEMPERATURECPARP%</span>
    <sup class="units">&deg;C</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturecPz").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturecPz", true);
  xhttp.send();
}, 2000) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturecPp").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturecPp", true);
  xhttp.send();
}, 2000) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturecParZ").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturecParZ", true);
  xhttp.send();
}, 2000) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperaturecParP").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperaturecParP", true);
  xhttp.send();
}, 2000) ;

</script>
</html>)rawliteral";

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
}