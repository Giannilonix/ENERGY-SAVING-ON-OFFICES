/*

http://ledcontroller.local/
http://ledcontroller.local/status

http://ledcontroller.local/fc_1_on
http://ledcontroller.local/fc_1_off
http://ledcontroller.local/fc_2_on
http://ledcontroller.local/fc_2_off
http://ledcontroller.local/fc_3_on
http://ledcontroller.local/fc_3_off
http://ledcontroller.local/fc_4_on
http://ledcontroller.local/fc_4_off
http://ledcontroller.local/fc_5_on
http://ledcontroller.local/fc_5_off

http://ledcontroller.local/l_1_on
http://ledcontroller.local/l_1_off
http://ledcontroller.local/l_2_on
http://ledcontroller.local/l_2_off
http://ledcontroller.local/l_3_on
http://ledcontroller.local/l_3_off

http://ledcontroller.local/all_fc_on
http://ledcontroller.local/all_fc_off

http://ledcontroller.local/all_lights_on
http://ledcontroller.local/all_lights_off

*/
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#define FC_1 13
#define FC_2 5
#define FC_3 2
#define FC_4 27
#define FC_5 25

#define LIGHT_1 10
#define LIGHT_2 9
#define LIGHT_3 26

//#define FC_ARRAY [5]  {34,39,36,18,19}
int FC_array [5] = {FC_1,FC_2,FC_3,FC_4,FC_5};
int LIGHT_array [3] = {LIGHT_1,LIGHT_2,LIGHT_3};
String FC_STATUS [5]= {"off","off","off","off","off"};
String LIGHT_STATUS [3]= {"off","off","off"};

const char* ssid = "AMammellata";
const char* password = "123lonia456";

// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);

void setup(void)
{  
    pinMode(FC_1, OUTPUT);
    pinMode(FC_2, OUTPUT);
    pinMode(FC_3, OUTPUT);
    pinMode(FC_4, OUTPUT);
    pinMode(FC_5, OUTPUT);

    pinMode(LIGHT_1, OUTPUT);
    pinMode(LIGHT_2, OUTPUT);
    pinMode(LIGHT_3, OUTPUT);
    
    digitalWrite(FC_1,0);
    digitalWrite(FC_2,0);
    digitalWrite(FC_3,0);
    digitalWrite(FC_4,0);
    digitalWrite(FC_5,0);
    
    digitalWrite(LIGHT_1,0);
    digitalWrite(LIGHT_2,0);
    digitalWrite(LIGHT_3,0);

   
    Serial.begin(115200);

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

    if (!MDNS.begin("ledcontroller")) {
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
}

 
void loop(void)
{
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
      s = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";  
      s += "{ \"FC_1\":\"" + FC_STATUS[0] + "\"," + "\"FC_2\":\"" + FC_STATUS[1] + "\"," + "\"FC_3\":\"" + FC_STATUS[2] + "\"," + "\"FC_4\":\"" + FC_STATUS[3] + "\"," +"\"FC_5\":\"" + FC_STATUS[4] + "\"," + "\"LIGHT_1\":\"" + LIGHT_STATUS[0] + "\"," + "\"LIGHT_2\":\"" + LIGHT_STATUS[1] + "\"," + "\"LIGHT_3\":\"" + LIGHT_STATUS[2] + "\"" + "}";
      Serial.println("Sending json");
      }
    else if(req=="/fc_1_on"){
      digitalWrite(FC_1,1); 
      FC_STATUS [0]= "on";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED ON</html>\r\n\r\n";
      }
    else if(req=="/fc_1_off"){
      digitalWrite(FC_1,0); 
      FC_STATUS [0]= "off";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED OFF</html>\r\n\r\n";
      }
    else if(req=="/fc_2_on"){
      digitalWrite(FC_2,1); 
      FC_STATUS [1]= "on";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED ON</html>\r\n\r\n";
      }
    else if(req=="/fc_2_off"){
      digitalWrite(FC_2,0); 
      FC_STATUS [1]= "off";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED OFF</html>\r\n\r\n";
      }
    else if(req=="/fc_3_on"){
      digitalWrite(FC_3,1); 
      FC_STATUS [2]= "on";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED ON</html>\r\n\r\n";
      }
    else if(req=="/fc_3_off"){
      digitalWrite(FC_3,0); 
      FC_STATUS [2]= "off";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED OFF</html>\r\n\r\n";
      }
    else if(req=="/fc_4_on"){
      digitalWrite(FC_4,1); 
      FC_STATUS [3]= "on";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED ON</html>\r\n\r\n";
      }
    else if(req=="/fc_4_off"){
      digitalWrite(FC_4,0); 
      FC_STATUS [3]= "off";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED OFF</html>\r\n\r\n";
      }
    else if(req=="/fc_5_on"){
      digitalWrite(FC_5,1); 
      FC_STATUS [4]= "on";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED ON</html>\r\n\r\n";
      }
    else if(req=="/fc_5_off"){
      digitalWrite(FC_5,0); 
      FC_STATUS [4]= "off";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED OFF</html>\r\n\r\n";
      }
    else if(req=="/l_1_on"){
      digitalWrite(LIGHT_1,1); 
      LIGHT_STATUS [0]= "on";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED ON</html>\r\n\r\n";
      }
    else if(req=="/l_1_off"){
      digitalWrite(LIGHT_1,0); 
      LIGHT_STATUS [0]= "off";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED OFF</html>\r\n\r\n";
      }
    else if(req=="/l_2_on"){
      digitalWrite(LIGHT_2,1); 
      LIGHT_STATUS [1]= "on";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED ON</html>\r\n\r\n";
      }
    else if(req=="/l_2_off"){
      digitalWrite(LIGHT_2,0); 
      LIGHT_STATUS [1]= "off";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED OFF</html>\r\n\r\n";
      }
    else if(req=="/l_3_on"){
      digitalWrite(LIGHT_3,1); 
      LIGHT_STATUS [2]= "on";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED ON</html>\r\n\r\n";
      }
    else if(req=="/l_3_off"){
      digitalWrite(LIGHT_3,0); 
      LIGHT_STATUS [2]= "off";
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED OFF</html>\r\n\r\n";
      }

    else if(req=="/all_fc_on"){
      for(int i =0;i<5;i++){
        digitalWrite(FC_array[i],1);
        FC_STATUS[i]="on";
      }
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>All Fan coils switched ON!</html>\r\n\r\n";
      }
    else if(req=="/all_fc_off"){
      for(int i =0;i<5;i++){
        digitalWrite(FC_array[i],0);
        FC_STATUS[i]="off";
      }
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>All Fan coils switched OFF!</html>\r\n\r\n";
      }
    else if(req=="/all_lights_on"){
      for(int i =0;i<3;i++){
        digitalWrite(LIGHT_array[i],1);
        LIGHT_STATUS[i]="on";
      }
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>All Lights switched ON!</html>\r\n\r\n";
      }
    else if(req=="/all_lights_off"){
      for(int i =0;i<3;i++){
        digitalWrite(LIGHT_array[i],0);
        LIGHT_STATUS[i]="off";
      }
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>All Lights switched OFF!</html>\r\n\r\n";
      }
      
    else{
      s = "HTTP/1.1 404 Not Found\r\n\r\n";
      Serial.println("Sending 404");
      }

    client.println(s);
   
    client.stop();
    client.flush();
    Serial.println("Done with client");
}


/*
else if (req == "/on")
    { 
        digitalWrite(led,1); 
        led_status = "on";
        s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED ON</html>\r\n\r\n";
        Serial.println("Sending 200");
    }

    else if (req == "/off")
    { 
        digitalWrite(led,0); 
        led_status = false;
        s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>LED OFF</html>\r\n\r\n";
        Serial.println("Sending 200");
    }

*/
