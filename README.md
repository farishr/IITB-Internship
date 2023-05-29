# IIT-B Internship
This repository contains all the projects I developed during my internship at IIT Bombay. 

## ESP32 Projects
All the libraries and tools used to develop the projects within this repository can be found in the "Library & Tools" folder. Code has been references from multiple sources. **References can be found at the end of this document.**

<ins>**Specifications of ESP32**<ins/>
<br>SPI Interfaces:<br/>
![SPI Interfaces of ESP32](/MISC/SPI_Interfaces_ESP32.png)


### 1. Hosting a website
This project is a simple website hosted on the ESP32 Microcontroller. The website is hosted on an Async Webserver and the ESP32 is configured in Access Point mode. The HTML files for the website are stored on the SPIFFS memory of the ESP32. These files can be found in the "data" folder within the sketch folder.
<br>**Sketch: [sample_website.ino](/sample_website/sample_website.ino)**

### 2. Website with Login
Similar to project 1, a simple website is hosted on the ESP32 Async Webserver. In addition, a login page is requested and the ESP32 operates in STA (Station Point Mode). ESP32 and Device needs to be on the same network. Networks Credetials need to be hardcoded in the Firmware.
<br>**Sketch: [website_w_login.ino](/website_w_login/website_w_login.ino)**
  

### 3. MPU9250 Data Acquistion & SD Card Storage using Free-RTOS
In this project, MPU9250 data is collected and stored into the SD Card simulataneously using the multi-core functionality of the ESP32 Microcontroller.
<br>**Sketch: [mpu_sd_rtos.ino](/mpu_sd_rtos/mpu_sd_rtos.ino)**

#### Components Used:
1. MPU9250 (9-Axis (Gyro + Accelerometer + Magnetometer) MEMS MotionTracking Device)
2. Micro SD Card Reader Module

#### Wiring:
![MPU and SD Card Wiring](/MISC/mpu_sd_wiring.png)

Both MPU and SD Card uses SPI communication and is connected to the ESP32 via HSPI & VSPI respectively. Core 0 retrieves data from the MPU into an array and Core 1 writes this data into the SD Card. The global array is split into _n_ sections. While MPU data is stored into the _n<sup>th<sup/>_ section of the array, data in the _(n-1)<sup>th<sup/>_ section is written into the SD Card. Here, the maximum data accquisition speed is determined by the slowed device which is the SD Card Module. In addition, as the file size increases, opening and closing of the file takes longer time as a result, the read and write speed gradually decreases. This can be tackled by creating a new file after a specific number of samples have been recorded.
**Observations:** Maximum data acquisiton speed: 2KHz (1 sample = 1 row = 6 data points)
  
  
### 4. Heap Allocation using Free-RTOS (Dynamic Memory)
This sketch implements a simple heap allocation application. There are two different functions running simultaneously on both the cores of ESP32. Core 0 runs a function which continually monitors the Serial monitor for input from the user and Core 1 runs the function that prints the user's text. Once the user submits the input into the serial monitor, heap is allocated for this input string and is read by the second function and printed on the serial monitor. 
<br>**Refer: [Free-RTOS Guide](https://www.freertos.org/fr-content-src/uploads/2018/07/161204_Mastering_the_FreeRTOS_Real_Time_Kernel-A_Hands-On_Tutorial_Guide.pdf)**
<br>**Sketch: [heap_allocation_rtos.ino](/heap_allocation_rtos/heap_allocation_rtos.ino)**
  
### 5. SD Card Read and Write Benchmarking
The sketches in this project determine the maximum read and write speed of the micro-SD Card module using SPI protocol. The results of benchmarking can be found in this sheet - **SD Card Benchmarking Results.ods.** The SD Card module uses the VSPI channel of the ESP32. The ESP32 is compatible with FAT32 file system. exFat32 file systems are not supported or can conflict with ESP32.<br/>
<br>Further tests can be done by changing the SPI frequency and writing data in bindary. Setting a higher SPI frequency has been done in the previous project - _MPU9250 Data Acquistion & SD Card Storage using Free-RTOS_
![SD Card Benchmarking Results](/MISC/SD_BENCH_RESULTS.png)
<br>**Sketch Folder: SD Card Benchmarking**

### 6. SD Card Data read + send over HTTP to PHP Server
This sketch reads a file from a micro-SD card and sends the file to a php server via HTTP as POST requests. SD Card is connected to the ESP32 through the VSPI interface. A remote php server is hosted and the ESP32 is configured in client mode. The ESP can either connect directly to the server or be on the same network as the server. The POST requests sent from the ESP32 are collected and appended into a file. This is done by writing a custom php script at the server side.
<br>**Sketch: ![sddatafile_to_php_packets.ino](sddatafile_to_php_packets/sddatafile_to_php_packets.ino)**
<br>**PHP Script: ![espdata_sdcard.php](php_scripts/espdata_sdcard.php)**

### 7. HTTP Transfer Rate Test (Data as a String)
This sketch is used to test the HTTP tranfer speed from ESP32 to a remote PHP Server. The data is sent from ESP32 as a string and received at the server side. The server side PHP script receives this data and stores it in a text file. Here, 7 channel(4 digit each) is sent over continously. I have found that for this type of data, transfer rate is approximately **~2000Hz** where each sample is 1 row of data. 
<br>_Note: The transfer speed of HTTP requests depends on multiple factors such as router bandwidth, exact character count, proximity to the router, network etc. The transfer rate can vary according to specific applications._
<br>**Sketch: ![HTTPTransferSpeed.ino](HTTPTransferSpeed/HTTPTransferSpeed.ino)**
<br>**PHP Script: ![post-esp-data_v2.php](php_scripts/post-esp-data_v2.php)**
  
### 8. MPU9250 interfacing with Raspberry Pi 4 using I2C
This project contains files that interface Raspberry Pi 4's I2C channel with the MPU9250 to read accelerometer and gyroscope sensor values. This sketch is very similar to the esp32 sketch for the same. Benchmarking files are also included in this project.  
<br> **Sketch: ![i2c_mpu9250.py](MPU9250_RPI/i2c_mpu9250.py)**
<br> **Benchmarking Results:**

