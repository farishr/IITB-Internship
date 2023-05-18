/*
This program simulataneously reads data from the MPU9250 and writes this data into the SD Card using Free-RTOS.

Maximum data acquisiton speed: 2Khz (1 sample = 1 row = 6 data points)
Data is written into a text file.
With increase in file size, the write speed reduces as file open & close takes more time.

*/


#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_wdt.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"

#include <MPU9250_WE.h>
#include <SPI.h>
#include <SD.h>

#define LED 2   //for overflow check

/////////////MPU Setup/////////////
#define MPU_CS_PIN        27
#define MPU_CLK_PIN       14
#define MPU_MOSI_PIN      13
#define MPU_MISO_PIN      12

SPIClass* hspi = new SPIClass(HSPI);

float Ax[256] {};
float Ay[256] {};
float Az[256] {};
float Gx[256] {};
float Gy[256] {};
float Gz[256] {};
uint32_t s_no[256] {};
int read_time[256] {};

uint32_t slno {1};
uint32_t rstart=0, rtime=0, wstart=0, wtime=0;


String data = "";

bool error_check = false;
const int csPin = 5;  // Chip Select Pin
bool useSPI = true;    // SPI use flag
bool write_ready_flag = false;
//bool read_start_flag = true;
//bool first_run = true;

int counter = 0;
//int multiplier = 1;

int sd_count = 0;
//int sd_multi = 1;

float Acc_bias[3];
float Gyro_bias[3];
const int NUM_OF_SAMPLES = 64;

MPU9250_WE myMPU9250 = MPU9250_WE(hspi, MPU_CS_PIN, useSPI);

unsigned long lastMicros = 0;
const int MAX_SAMPLING_FREQ = 500;
unsigned long MINIMUM_SAMPLING_DELAY_uSec = (unsigned long)(1 * 1000000 / MAX_SAMPLING_FREQ);

/////////////Function Prototypes/////////////
void calibrate_MPU(float acc_bias[], float gyro_bias[]);
void writeFile(fs::FS &fs, const char * path, const char * message, bool &check);
void appendFile(fs::FS &fs, const char * path, String message);
void deleteFile(fs::FS &fs, const char * path, bool &check);

/////////////FREE-RTOS Setup/////////////
TaskHandle_t MPU_GET_h = NULL, SD_Write_h = NULL;

/////////////PARALLEL TASKS/////////////

void MPU_GET( void * parameter ) {
  esp_task_wdt_delete(NULL);

  vTaskSuspend(NULL);

  hspi->begin(MPU_CLK_PIN, MPU_MISO_PIN, MPU_MOSI_PIN, MPU_CS_PIN);//SCLK, MISO, MOSI, SS
  if (!myMPU9250.init()) {
    Serial.println("MPU9250 does not respond");
  } else {
    Serial.println("MPU9250 is connected");
  }

  delay(1000);
  myMPU9250.autoOffsets();
  myMPU9250.disableGyrDLPF(MPU9250_BW_WO_DLPF_8800); // bandwdith without DLPF
  myMPU9250.setGyrRange(MPU9250_GYRO_RANGE_250);
  myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G);
  myMPU9250.enableAccDLPF(false);
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);
  myMPU9250.setMagOpMode(AK8963_CONT_MODE_100HZ);
  delay(200);
  calibrate_MPU(Acc_bias, Gyro_bias);
  Serial.println("Calibration Done! Starting Data Acquisiton.");
  delay(1000);

  for (;;) {
    esp_task_wdt_init(5, false);

    for (int i = 0; i <= 3; i++) {
    
      rstart = micros();

      for (counter = i * NUM_OF_SAMPLES; counter < (i + 1) * NUM_OF_SAMPLES; counter ++) {
        if ((micros() - lastMicros) >= MINIMUM_SAMPLING_DELAY_uSec) {
            lastMicros = micros();
            xyzFloat gValue = myMPU9250.getGValues();
            xyzFloat gyr = myMPU9250.getGyrValues();

            s_no[counter] = slno;
            Ax[counter] = (gValue.x - Acc_bias[0]);
            Ay[counter] = (gValue.y - Acc_bias[1]);
            Az[counter] = (gValue.z - Acc_bias[2]);
            Gx[counter] = (gyr.x - Gyro_bias[0]);
            Gy[counter] = (gyr.y - Gyro_bias[1]);
            Gz[counter] = (gyr.z - Gyro_bias[2]);
            read_time[counter] = rtime;
            
            slno++;
        } else {
            counter--;
        }
      }
      if (!write_ready_flag) {
        write_ready_flag = true;
        vTaskResume(SD_Write_h);
      } else {
        digitalWrite(LED,HIGH);
        Serial.println("!!!OVERFLOW!!!");
        while(1){delay(1000);}
      }

      rtime = micros()-rstart;

    }
  }
}

void SD_Write( void * parameter ) {
  //int weare = 1;
  //String wee;
  esp_task_wdt_delete(NULL);
  vTaskSuspend(NULL);

  for (;;) {

    esp_task_wdt_init(5, false);

    for (int sd_count = 0; sd_count < 4; sd_count++) {
    
      wstart = micros();

      for (int index = sd_count * NUM_OF_SAMPLES; index < (sd_count + 1) * NUM_OF_SAMPLES; index ++) {
        
        data += (String)s_no[index] + ","  +
                (String)Ax[index] + "," +
                (String)Ay[index] + "," +
                (String)Az[index] + "," +
                (String)Gx[index] + "," +
                (String)Gy[index] + "," +
                (String)Gz[index] + "," +
                (String)read_time[index] + "," +
                (String)(wtime) + "\n";

        //Serial.printf("%d,%f,%f,%f,%f,%f,%f\n", weare,Ax[index],Ay[index],Az[index],Gx[index],Gy[index],Gz[index]);

      }

      appendFile(SD, "/bench_SDwrite.txt", data);
      data = "";
      //Serial.print(data);
      write_ready_flag = false;

      wtime = micros()-wstart;
      vTaskSuspend(NULL);

    }
  }
}


/////////////MAIN SETUP FUNCTION/////////////
void setup() {
  Serial.begin(115200);

    pinMode(LED,OUTPUT);
    digitalWrite(LED,LOW);

    if (!SD.begin(csPin, SPI, 18000000)) {  //frequency beyond 19Mhz will cause ESP to restart.
        Serial.println("Card Mount Failed");
        return;
    }

    deleteFile(SD, "/bench_SDwrite.txt", error_check);
    writeFile(SD, "/bench_SDwrite.txt", "S.No,Ax,Ay,Az,Gx,Gy,Gz,MPURead_Time,SdWrite_Time\n", error_check);

    Serial.println("All Setups are complete. Starting Tasks.");

    xTaskCreatePinnedToCore(
        MPU_GET,     //TASK
        "MPU_GET",   //Task id
        8000,               //Stack Size
        NULL,
        1,
        &MPU_GET_h,   //TASK HANDLE
        0
    );

    xTaskCreatePinnedToCore(
        SD_Write,
        "SD_Write",   //Task ID
        8000,               //Stack Size
        NULL,
        1,
        &SD_Write_h,   //TASK HANDLE
        1
    );

    vTaskResume(MPU_GET_h);
    vTaskDelete(NULL);

}

void loop() {
}


void calibrate_MPU(float acc_bias[], float gyro_bias[]) {  //pass array by ref

  const int MPU9250_MINIMUM_SAMPLING_DELAY_uSEC = 250;  //250 because max sampling rate of accelerometer is 4 khz

  for (int i = 0; i < 3; i++) {
    gyro_bias[i] = 0.0;
    acc_bias[i] = 0.0;
  }

  for (int i = 0; i < NUM_OF_SAMPLES; i++) {
    xyzFloat gValue = myMPU9250.getGValues();
    xyzFloat gyr = myMPU9250.getGyrValues();

    acc_bias[0] = acc_bias[0] + gValue.x;
    acc_bias[1] = acc_bias[1] + gValue.y;
    acc_bias[2] = acc_bias[2] + gValue.z;
    gyro_bias[0] = gyro_bias[0] + gyr.x;
    gyro_bias[1] = gyro_bias[1] + gyr.y;
    gyro_bias[2] = gyro_bias[2] + gyr.z;
    delayMicroseconds(MPU9250_MINIMUM_SAMPLING_DELAY_uSEC); //delay because max sampling rate of accelerometer is 4 khz
  }

  for (int i = 0; i < 3; i++) {
    gyro_bias[i] = gyro_bias[i] / float(NUM_OF_SAMPLES);
    acc_bias[i] = acc_bias[i] / float(NUM_OF_SAMPLES);
  }
  acc_bias[2] = acc_bias[2] + 1; //z axis

}

void writeFile(fs::FS & fs, const char * path, const char * message, bool & check) {
  //Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (!file.print(message)) {
    check = true;
  }
  file.close();
}

void appendFile(fs::FS & fs, const char * path, String message) {
  //Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    return;
  }
  file.print(message);
  file.close();
}

void deleteFile(fs::FS & fs, const char * path, bool & check) {
  Serial.printf("Deleting file: %s\n", path);
  if (!fs.remove(path)) {
    check = true;
  }
}