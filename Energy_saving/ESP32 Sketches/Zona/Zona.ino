/* 
http://zonaN.local/
http://zonaN.local/status
http://zonaN.local/timer_10s
http://zonaN.local/timer_3m
http://zonaN.local/timer_5m
http://zonaN.local/timer_10m

*/
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "AMammellata";
const char* password = "123lonia456";

const char* DNS_name = "zona3";

// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);
// GPIO where the DS18B20 is connected to
const int oneWireBus = 16;     
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

long rand_lux, Timer;
//variables to keep track of the timing of recent interrupts 
unsigned long last_presence_time = 0; 
bool presence = false;

String stato="False";

void IRAM_ATTR isr() {
    last_presence_time = millis();
    presence = true;
    Serial.println("Interrupt triggered");
}

void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);
  // Start the DS18B20 sensor
  sensors.begin();
  pinMode(13, INPUT_PULLUP);
  attachInterrupt(13, isr, RISING);
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
//------------------------------DNS NAME----------------------------------------------
  if (!MDNS.begin(DNS_name)) {
      Serial.println("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  Serial.println("mDNS responder started");

  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  //Presence Timer default value 1 minute but it is possible to adapt it 
  Timer = 60000;
}

void loop() {
  // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) {
        return;
    }
    Serial.println("");
    Serial.println("New client");
    // Wait for data from client to become available
    while(client.connected() && !client.available()){
        delay(1);
        //Serial.println("Waiting client");
    }
    // Read the first line of HTTP request
    String req = client.readStringUntil('\r');
    // First line of HTTP request looks like "GET /path HTTP/1.1"
    // Retrieve the "/path" part by finding the spaces
    int addr_start = req.indexOf(' ');
    int addr_end = req.indexOf(' ', addr_start + 1);
    if (addr_start == -1 || addr_end == -1) {
        Serial.print("Invalid request: ");
        Serial.println(req);
        return;
    }
    req = req.substring(addr_start + 1, addr_end);
    Serial.print("Request: ");
    Serial.println(req);

    String s;

    if(req=="/"){
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ESP32 at ";
      s += ipStr;
      s += "</html>\r\n\r\n";
      Serial.println("Sending 200");
      }
    else if(req=="/status"){
      //check_presence();
      sensors.requestTemperatures(); 
      float temperatureC = sensors.getTempCByIndex(0);
      rand_lux=random(399,402);
      if (presence && (millis() - last_presence_time) > Timer){
        presence = false;
        Serial.println("Presence setted to false");
      }
      if (presence)
        stato = "True";
      else
        stato = "False";

      s = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"; 
      s += "{ \"Temp\":\"" + String(temperatureC) + "\","+ "\"Presence\":\"" + stato + "\","+"\"Lux\":\"" + rand_lux + "\""+"}";
      //s += "{ \"Temp\":\"" + String(temperatureC) + "\"," + "\"FC_2\":\"" + FC_STATUS[1] + "\"," + "\"FC_3\":\"" + FC_STATUS[2] + "\"," + "\"FC_4\":\"" + FC_STATUS[3] + "\"," +"\"FC_5\":\"" + FC_STATUS[4] + "\"," + "\"LIGHT_1\":\"" + LIGHT_STATUS[0] + "\"," + "\"LIGHT_2\":\"" + LIGHT_STATUS[1] + "\"," + "\"LIGHT_3\":\"" + LIGHT_STATUS[2] + "\"," + "}";
      Serial.println("Sending json");
      }
    else if(req=="/timer_10s"){
      Timer = 10000;
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Presence Timer setted to 10 seconds </html>\r\n\r\n";
      Serial.println("Sending 200");
      }
    else if(req=="/timer_3m"){
      Timer = 180000;
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Presence Timer setted to 3 minutes </html>\r\n\r\n";
      Serial.println("Sending 200");
      }
    else if(req=="/timer_5m"){
      Timer = 300000;
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Presence Timer setted to 5 minutes </html>\r\n\r\n";
      Serial.println("Sending 200");
      }
    else if(req=="/timer_10m"){
      Timer = 600000;
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Presence Timer setted to 10 minutes </html>\r\n\r\n";
      Serial.println("Sending 200");
      }
    else{
      s = "HTTP/1.1 404 Not Found\r\n\r\n";
      Serial.println("Sending 404");
      }

    client.println(s);
   
    client.stop();
    client.flush();
    Serial.println("Done with client");
    delay(20);
}