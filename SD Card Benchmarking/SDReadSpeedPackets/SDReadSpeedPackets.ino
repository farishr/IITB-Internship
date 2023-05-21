/*

Calculating Read Speed of the SD Card. REF data set: test_data.csv
'test_data.csv' contains 6 channels of the number '9999' and 40000 rows. 
Here, the data is stored into a string.

Frequency = 1630Hz

*/
#include <SPI.h>
#include <SD.h> 

int buf_size = 4096;

void setup() {

  Serial.begin(115200);

  if(!SD.begin()){
      Serial.println("Card Mount Failed");
      return;
  }

  String data_pkt {};
  char ascii {};

  Serial.println("Beginning Reading.");
  long start_time = micros();

  File file = SD.open("/test_data.csv");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  while(file.available()){

    data_pkt = "";

    for (int i {0}; i < buf_size-1; i++){
      if(file.available()){
        ascii = (int)file.read();     //The read operation is done for each char because as packets when the data is over, it sends over NUL values.
        data_pkt += ascii;
      } else {break;}
    }
    //Serial.println(data_pkt.length());
      
  }

  file.close();
  long end_time = micros();

  // Serial.println(data_pkt);
  Serial.print("Read Complete. ");
  Serial.print("Read Time (micros): ");Serial.println(end_time-start_time);

}

void loop() { 
}