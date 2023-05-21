/*

Calculating Read Speed of the SD Card. REF data set: test_data.csv
SD Card Module uses VSPI channel

'test_data.csv' contains 6 channels of the number '9999' and 40000 rows. 

Here, the data is stored in a char array in ascii code.
Frequency = 1705Hz

*/
#include <SPI.h>
#include <SD.h> 

int counter {0};

void setup() {

  Serial.begin(115200);

  if(!SD.begin()){
      Serial.println("Card Mount Failed");
      return;
  }

  char data {};   //char array is significantly faster than string.

  Serial.println("Beginning Reading.");

  /*Reading*/
  long start_time = micros();   //START TIME
  File file = SD.open("/test_data.csv");

  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  while(file.available()){
    data = file.read();
    counter++;
  }

  file.close();
  long end_time = micros();   //END TIME

  Serial.print("Read Complete. Number of Characters: ");Serial.println(counter);
  Serial.print("Read Time (micros): ");Serial.println(end_time-start_time);

}

void loop() {

}