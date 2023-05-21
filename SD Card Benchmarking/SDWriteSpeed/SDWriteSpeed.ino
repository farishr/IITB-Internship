/*
SDWriteSpeed - This sketch writes a fixed number of samples into the SD Card using SPI interface of the ESP32. The data is written into the 'write_data.csv' file.

*/

#include "FS.h"
#include "SD.h"
#include "SPI.h"

const char * path = "/write_data.csv";
int n {10000};      //NUMBER OF ROWS
char text [24];

void setup(){
    Serial.begin(115200);

    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }

    File file = SD.open(path,FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    file.print("A,B,C,D,E,F\n");
    file.close();

    unsigned long start_time = micros();        //START TIME

    file = SD.open(path,FILE_APPEND);
    while(n != 0){
        sprintf(text,"%d,%d,%d,%d,%d,%d\n", 9999, 9999, 9999, 9999, 9999, 9999);
        file.print(text);
        n--;
    }
    file.close();

    unsigned long end_time = micros();          //END TIME

    Serial.print("Write speed in uS: ");Serial.println(end_time-start_time);
    vTaskDelete(NULL);  //Deletes the loop function.

}

/*Calculation of Write Frequency*

    Freq = n / (end_time-start_time)*1000000 secs

*/

void loop(){



}