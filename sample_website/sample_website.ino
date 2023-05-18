#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "FS.h"

#define FORMAT_SPIFFS_IF_FAILED false

//WiFi Setup
/*const char* ssid {"farishr"};
const char* password  {"freheman0719"};*/
const char *soft_ap_ssid = "ESP_FR";
const char *soft_ap_password = "testpassword";

//Server Setup
AsyncWebServer myESPserver(80);

//Function Proto
void ls(fs::FS &fs, const char* dirname, uint8_t levels);

void setup() {

  Serial.begin(115200);
  
  //SPIFFS Setup
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  //Connect to WiFi
  /*WiFi.begin(ssid, password);
  Serial.print("Connecting to Wifi");
  while(WiFi.status() != WL_CONNECTED){
  Serial.print(".");
  delay(1000);
  }
  Serial.print("\nConnected. Your Website IP: ");
  Serial.println(WiFi.localIP());*/
  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  Serial.print("ESP32 IP as soft AP: ");
  Serial.println(WiFi.softAPIP());
  
  ls(SPIFFS, "/", 0);

    // Route for root / web page
 myESPserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index2.html", String(), false);
  });
  
  myESPserver.on("/norway", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/norway.html", String(), false);
  });
  
  myESPserver.on("/uk", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/uk.html", String(), false);
  });
  
  myESPserver.on("/india", HTTP_GET, [](AsyncWebServerRequest *request){ 
    request->send(SPIFFS, "/india.html", String(), false);
  });

  myESPserver.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

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
