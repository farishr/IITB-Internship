#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED false

// Replace with your network credentials
const char* ssid = "IITB_IOT";
const char* password = "iitbiot1234
";

const char* http_username = "admin1";
const char* http_password = "admin1";

// Create AsyncWebServer object on port 80
AsyncWebServer myESPserver(80);

//Function Proto
void ls(fs::FS &fs, const char* dirname, uint8_t levels);

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  //SPIFFS Setup
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  ls(SPIFFS, "/", 0);

  // Route for root / web page
  myESPserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(SPIFFS, "/index2.html", String(), false);
  });
    
  myESPserver.on("/norway", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(SPIFFS, "/norway.html", String(), false);
  });
  
  myESPserver.on("/uk", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(SPIFFS, "/uk.html", String(), false);
  });
  
  myESPserver.on("/india", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(SPIFFS, "/india.html", String(), false);
  });

  myESPserver.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(401);
  });

  myESPserver.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/logout_page.html", String(), false);
  });

  // Send a GET request to <ESP_IP>/update?state=<inputMessage>
  /*server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      digitalWrite(output, inputMessage.toInt());
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });*/
  
  // Start server
  myESPserver.begin();
}
  
void loop() {
  
}

void ls(fs::FS &fs, const char* dirname, uint8_t levels){

  Serial.printf("Listing SPIFFS Directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if(!root){
      Serial.println("Unable to Open Directory.");
      return;
    }
  if(!root.isDirectory()){
      Serial.println("Invalid Directory.");
      return;
    }

  File file = root.openNextFile();

  while(file){

    if(file.isDirectory()){

        Serial.print("  DIR:");
    Serial.println(file.name());
    
    if(levels)
      ls(fs, file.name(), levels - 1);
    } else {
    Serial.print("  FILE: ");
    Serial.print(file.name());
    Serial.print("\tSIZE: ");
    Serial.print(file.size());
    Serial.println(" bytes");   
  }
  file = root.openNextFile();   
  }  
}
