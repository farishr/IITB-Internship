# IIT-B Internship
This repository contains all the projects I developed during my internship at IIT Bombay. 

## ESP32 Projects
All the libraries and tools used to develop the projects within this repository can be found in the "Library & Tools" folder. Code has been references from multiple sources. **References can be found at the end of this document.**

### 1. Hosting a website
This project is a simple website hosted on the ESP32 Microcontroller. The website is hosted on an Async Webserver and the ESP32 is configured in Access Point mode. The HTML files for the website are stored on the SPIFFS memory of the ESP32. These files can be found in the "data" folder within the sketch folder.
<br />**Sketch: sample_website**<br />

### 2. Website with Login
Similar to project 1. Here, a login page is requested and the ESP32 operates in STA (Station Point Mode). ESP32 and Device needs to be on the same network. Networks Credetials need to be hardcoded in the Firmware.
<br />**Sketch: website_w_login**<br />
