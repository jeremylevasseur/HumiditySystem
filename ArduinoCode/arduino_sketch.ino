#include <ESP8266HTTPClient.h>

//#include <ESP8266WebServer-impl.h>
#include <ESP8266WebServer.h>
//#include <ESP8266WebServerSecure.h>
//#include <Parsing-impl.h>
#include <Uri.h>

// #include <ESP8266WiFi.h>
#include "DHT.h"
#include <Hash.h>



// Uncomment one of the lines below for whatever DHT sensor type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

/*Put your SSID & Password*/
const char* ssid = "BELL473";  // Enter SSID here
const char* password = "79123ADEEF94";  //Enter Password here

const char* base_api_url = "http://192.168.2.51:8085/api/update_data/";

// DHT Sensor
uint8_t DHTPin = D8; 
               
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);                

float t=0.0;
float h=0.0;

ESP8266WebServer server(80);
void handleRoot(); 
void handleTemp(); 
void handleHumid(); 

  void handleRoot() {
  server.send(200, "text/plain", "");
  }
  void handleTemp() {
  server.send(200, "text/plain", String(t).c_str());   // Send HTTP status 200 (Ok) and send some text to the browser/client
  }
  void handleHumid() {
  server.send(200, "text/plain", String(h).c_str());   // Send HTTP status 200 (Ok) and send some text to the browser/client
  }


// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0; // will store last time DHT was updated
// Updates DHT readings every 60 seconds
const long interval = 60000;

const char index_html[] PROGMEM = R"rawliteral(
      <!DOCTYPE HTML><html>
      <head>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
      integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9S
      r" crossorigin="anonymous">
      <style>
      html {
      font-family: Arial;
      display: inline-block;
      margin: 0px auto;
      text-align: center;
      }
      h2 { font-size: 3.0rem; }
      p { font-size: 3.0rem; }
      .units { font-size: 1.2rem; }
      .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
      }
      </style>
      </head>
      <body>
      <h2>ESP8266 DHT Server</h2>
      <p>
      <i class="fas fa-thermometer-half" style="color:#059e8a;"></i>
      <span class="dht-labels">Temperature</span>
      <span id="temperature">%TEMPERATURE%</span>
      <sup class="units">&deg;C</sup>
      </p>
      <p>
      <i class="fas fa-tint" style="color:#00add6;"></i>
      <span class="dht-labels">Humidity</span>
      <span id="humidity">%HUMIDITY%</span>
      <sup class="units">%</sup>
      </p>
      </body>
      <script>
      setInterval(function ( ) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
      }
      };
      xhttp.open("GET", "/temperature", true);
      xhttp.send();
      }, 10000 ) ;
      setInterval(function ( ) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
      }
      };
      xhttp.open("GET", "/humidity", true);
      xhttp.send();
      }, 10000 ) ;
      </script>
      </html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
    Serial.println(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
    Serial.println(h);
  }
  return String();
}



void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.begin();
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
  // Route for root / web page
  
  //Bridge.begin();
   
  server.on("/", handleRoot);
  server.on("/temperature", handleTemp);
  server.on("/humidity", handleHumid);
  
  
  // Start server
  server.begin();
}

void loop(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
    }
    
    HTTPClient http;

    String serverPath = "http://192.168.2.51:8085/api/update_data/cuisine/" + String(t) + "/" + String(h);
    // Serial.println(serverPath);
      
    // Your Domain name with URL path or IP address with path
    http.begin(serverPath.c_str());
    
    // Send HTTP GET request
    int httpResponseCode = http.GET();
    
    if (httpResponseCode>0) {
      // Serial.print("HTTP Response code: ");
      // Serial.println(httpResponseCode);
      String payload = http.getString();
      // Serial.println(payload);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
}

  
