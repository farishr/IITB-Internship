/*
    Sending Data from ESP32 to Server using only WiFi.h Library.
    Referencing PHP File: post-esp-data_v2.php

    v4: Using only WiFi.h Library
*/

#include <WiFi.h>
#include <SPI.h>
#include <SD.h> 

const char* ssid     = "ENTER_SSID";
const char* password = "ENTER_PASSWORD";
//bool ack = true;
int buf_size = 6144;

const char* server = "192.168.0.100"; // Server URL
//Update server details in the send_packets() function
void send_packets(String, const char*, int);

WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  delay(100);

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);
  
  if(!SD.begin()){
      Serial.println("Card Mount Failed");
      return;
    }

    String data_pkt {};
    char ascii {};
    Serial.println("Beginning Transmission.");

    // send_packets("data_packet=CounterValues\n", server, 80);    //Sending Header
    int start1 = micros();

    File file = SD.open("/test_data.csv");
    if (!file) {
      Serial.println("Failed to open file for reading");
      return;
    }

    while(WiFi.status() == WL_CONNECTED && file.available()){

      data_pkt = "data_packet=";

      for (int i {0}; i < buf_size-1; i++){
        if(file.available()){
          ascii = (int)file.read();     //The read operation is done for each char because as packets when the data is over, it sends over NUL values.
          data_pkt += ascii;
        } else {break;}
      }
      // Serial.print(data_pkt);
      send_packets(data_pkt, server, 80);
      
    }
    file.close();
    int end1 = micros();

    Serial.printf("\nTransmission Complete. TX Time: %d", end1-start1);
    String time = (String)(end1-start1);
    send_packets("data_packet=\nTX Time: " + time + "uS", server, 80);

}

void loop() {
  // do nothing
}

void send_packets(String packet, const char* server, int port){
    if (!client.connect(server, port)){
        Serial.println("Connection failed!");
    }
    client.println("POST /espdata_sdcard.php HTTP/1.1");    //HTTP/1.0 gives a slightly improved speed than HTTP3.0.
    // client.print("Host: 192.168.0.115:80");
    client.println("Host: 192.168.0.100:80");
    // client.print("Host: ");client.print(server);client.print(":");client.println(port);
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(packet.length());
    client.println();
    client.println(packet);
    client.stop();
    return;
}
