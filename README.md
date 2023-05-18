# IIT-B Internship
This repository contains all the projects I developed during my internship at IIT Bombay. 

## ESP32 Projects
All the libraries and tools used to develop the projects within this repository can be found in the "Library & Tools" folder. Code has been references from multiple sources. **References can be found at the end of this document.**

<ins>**Specifications of ESP32**<ins/>
<br>SPI Interfaces:<br/>
![SPI Interfaces of ESP32](/MISC/SPI_Interfaces_ESP32.png)


### 1. Hosting a website
This project is a simple website hosted on the ESP32 Microcontroller. The website is hosted on an Async Webserver and the ESP32 is configured in Access Point mode. The HTML files for the website are stored on the SPIFFS memory of the ESP32. These files can be found in the "data" folder within the sketch folder.
<br>**Sketch: `sample_website`**<br/>

### 2. Website with Login
_Similar to project 1, a simple website is hosted on the ESP32 Async Webserver. In addition, a login page is requested and the ESP32 operates in STA (Station Point Mode). ESP32 and Device needs to be on the same network. Networks Credetials need to be hardcoded in the Firmware.
<br>**Sketch: `website_w_login`**<br/>

### 3. MPU9250 Data Acquistion & SD Card Storage using Free-RTOS
In this project, MPU9250 data is collected and stored into the SD Card simulataneously using the multi-core functionality of the ESP32 Microcontroller.
<br>**Sketch: `mpu_sd_rtos`**<br/>

#### Components Used:
1. MPU9250 (9-Axis (Gyro + Accelerometer + Magnetometer) MEMS MotionTracking Device)
2. Micro SD Card Reader Module

#### Wiring:
![MPU and SD Card Wiring](/MISC/mpu_sd_wiring.png)

Both MPU and SD Card uses SPI communication and is connected to the ESP32 via HSPI & VSPI respectively. Core 0 retrieves data from the MPU into an array and Core 1 writes this data into the SD Card. The global array is split into _n_ sections. While MPU data is stored into the _n<sup>th<sup/>_ section of the array, data in the _(n-1)<sup>th<sup/>_ section is written into the SD Card. Here, the maximum data accquisition speed is determined by the slowed device which is the SD Card Module. In addition, as the file size increases, opening and closing of the file takes longer time as a result, the read and write speed gradually decreases. This can be tackled by creating a new file after a specific number of samples have been recorded.

**Observations:** Maximum data acquisiton speed: 2KHz (1 sample = 1 row = 6 data points)

