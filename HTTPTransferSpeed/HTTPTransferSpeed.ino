/*

This sketch transfers String values to a remote php server via HTTP POST requests.

*/
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_wdt.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"

#define LED 2

WiFiClient client;

const int N = 1024;
const int N_set_gather = 4;
const int N_gather_size = N / N_set_gather; // 1024/4 = 256. 256 gives better HTTP performance.

int16_t AcX[N], AcY[N], AcZ[N], GyX[N], Tmp[N], GyY[N], GyZ[N], TIME[N];
// unsigned long SL_NO[N];
bool send_Flag = true;

unsigned long lastMicros = 0;
const int MAX_SAMPLING_FREQ = 2000; //Maximum speed without overflow - 2000Hz
unsigned long MINIMUM_SAMPLING_DELAY_uSec = (unsigned long)(1*1000000/MAX_SAMPLING_FREQ);

String data = "data_packet=";
uint16_t count = 0;

//////////////Wi-Fi Credentials///////////////
const char* ssid = "ENTER_SSID";
const char* password =  "ENTER_PASS";
const char* server = "192.168.0.100"; // Server URL - Ubuntu LAN.


const int port = 80;

void send_packets(String, const char*, int);
void connect_to_wifi();

//////////////FREE-RTOS SETUP///////////////
TaskHandle_t MPU_GET_h = NULL, HTTP_SEND_h = NULL;

void MPU_GET( void * parameter ) {
    
    vTaskSuspend(NULL);

    while(1) {
        for (int j = 0; j < N_set_gather; j++) { //256 data points is 1 set and there are 4 sets
            for (int i = j * N_gather_size; i < (j + 1) * N_gather_size; i++) {
                if(micros() >= (lastMicros + MINIMUM_SAMPLING_DELAY_uSec)){
                    
                    // SL_NO[i] = count;
                    // count++;
                    TIME[i]= micros() - lastMicros;
                    lastMicros = micros();

                    AcX[i] = 9999;
                    AcY[i] = 9999;
                    AcZ[i] = 9999;
                    Tmp[i] = 9999;
                    GyX[i] = 9999;
                    GyY[i] = 9999;
                    GyZ[i] = 9999;

                } else {
                    i--;
                }
            }
            if (send_Flag) {
                send_Flag = false;
                vTaskResume(HTTP_SEND_h);
            } else {
                digitalWrite(LED,HIGH);
                Serial.println("!!!OVERFLOW!!!");
                while(1){delay(1000);}
            }

            esp_task_wdt_init(5, false); // Increases the timeout and feeds the TWDT.
            
        }
    }

}

void HTTP_SEND( void * parameter ) {
  vTaskSuspend(NULL);

  while(1) {
    for (int j = 0; j < N_set_gather; j++) {

        for (int i = j * N_gather_size; i < (j + 1) * N_gather_size; i++) {

            data += 
                // (String)(SL_NO[i]) + "," + `
                (String)(TIME[i]) + "," + 
                (String)(AcX[i]) + "," + 
                (String)(AcY[i]) + "," +
                (String)(AcZ[i]) + "," +
                (String)(Tmp[i]) + "," +
                (String)(GyX[i]) + "," +
                (String)(GyY[i]) + "," +
                (String)(GyZ[i]) + "\n";
        }
        if(WiFi.status() == WL_CONNECTED){
            send_packets(data, server, port);
            send_Flag = true;
        } else {
            while(1){
                Serial.println("!!WIFI DISCONNECT ERROR!!");
                digitalWrite(LED,HIGH);
                while(1){delay(1000);}
            }
        }
        data = "data_packet=";
        vTaskSuspend(NULL);
    }
    esp_task_wdt_init(5, false);  
    // esp_task_wdt_reset();
  }
}

void setup(){
    Serial.begin(115200);
    pinMode(LED,OUTPUT);
    digitalWrite(LED,LOW);
    
    delay(5000);
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }

    Serial.print("Connected to ");
    Serial.println(ssid);

    xTaskCreatePinnedToCore(MPU_GET, "MPU_GET",    10000,    NULL,    1,    &MPU_GET_h,   0);
    xTaskCreatePinnedToCore(HTTP_SEND, "HTTP_SEND",    10000,    NULL,    1,    &HTTP_SEND_h,   1);

    if(WiFi.status() == WL_CONNECTED){
        send_packets("data_packet=TIME,Acx,AcY,AcZ,TEMP,GyX,GyY,GyZ\n", server, port);
    } else {
        while(1){
            Serial.println("!!WIFI DISCONNECT ERROR!!");
            delay(1000);
        }
    }

    vTaskResume(MPU_GET_h);

}

void loop(){

  connect_to_wifi();

}

void send_packets(String packet, const char* server, int port){
    if (!client.connect(server, port)){
        Serial.println("Connection failed!");
        digitalWrite(LED,HIGH);
        while(1){delay(1000);}
    }
    client.println("POST /post-esp-data_v2.php HTTP/1.1");    //HTTP/1.0 gives a slightly improved speed than HTTP3.0.

    client.print("Host: ");client.print(server);client.print(":");client.println(port);
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

void connect_to_wifi() {
  unsigned long lastTimeStamp = millis();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    // Serial.println("Connecting to WiFi..");

    //if WiFI doesn't connect within 30 secs, reconnect WiFi
    if ( (millis() - lastTimeStamp) > 30*1000 ) {  
      // Serial.println("Failed to connect to WiFi. Restarting in 5 seconds");
      delay(5000);
      // ESP.restart();
	  WiFi.reconnect();
    }
  }
}