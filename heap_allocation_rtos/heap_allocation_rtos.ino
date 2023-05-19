/*

The idea here is to run two tasks on core 0 & 1. 

serialListen() - continually listens to use input from the Serial monitor
writeSerial() - Whenever the user enters a value, this task is triggered and the input is displayed back on the serial monitor 

*/

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

static const uint8_t buf_len = 100;

char c;
char buf[buf_len] {0};
static char *msg_ptr = NULL;    //for dynamic m/m allocation.
uint8_t idx = 0;
// Clear whole buffer

bool input_check_flag = false;

TaskHandle_t serialListen_h = NULL, writeSerial_h = NULL;

void serialListen( void * parameter ){

  //memset(buf, 0, buf_len);
  vTaskSuspend(NULL);

  // Loop forever
  while (1) {
    //char *ptr = (int*)pvPortMalloc(1024 * sizeof(char);)
    // Read characters from serial
    while(Serial.available() > 0 && !input_check_flag) {
      c = Serial.read();

      // Update delay variable and reset buffer if we get a newline character
      if (c == '\n') {
        input_check_flag = true;

        msg_ptr = (char *)pvPortMalloc(idx * sizeof(char)); //Allocating Memory
        // If malloc returns 0 (out of memory), throw an error and reset
        configASSERT(msg_ptr);
        // Copy message
        memcpy(msg_ptr, buf, idx);

        vTaskResume(writeSerial_h);
        vTaskSuspend(NULL);

        memset(buf, 0, buf_len);
        idx = 0;

        //Serial.println("serialListen() ends here.");

      } else {
        
        // Only append if index is not over message limit
        if (idx < buf_len - 1) {
          buf[idx] = c;
          idx++;
        }
      }
    }

    vTaskDelay( 100 / portTICK_PERIOD_MS); //Added so that the loop doesnt run as fast as it can. This will trigger the watchdog timer.
 }

}

void writeSerial( void * parameter ){
  while(1){
    if(!input_check_flag){
      vTaskSuspend(NULL);
    } else 
    {
      /*for(int i{0}; i < idx; i++)
        Serial.printf("%c", buf[i]);
      Serial.println();*/

      for(int j{0}; j < idx; j++)
        Serial.print(*(msg_ptr+j));
      
      Serial.println();
      
      vPortFree(msg_ptr);   //free the allocated memory
      msg_ptr = NULL;

      input_check_flag = false;
      vTaskResume(serialListen_h);
    }

    vTaskDelay( 100 / portTICK_PERIOD_MS);  //Added so that the loop doesnt run as fast as it can. This will trigger the watchdog timer.

  }

}

void setup(){

  Serial.begin(115200);
  vTaskDelay( 1000 / portTICK_PERIOD_MS );

  xTaskCreatePinnedToCore(
    serialListen,     //TASK
    "serialListen",   //Task id
    2048,               //Stack Size
    NULL,
    1,
    &serialListen_h,   //TASK HANDLE
    0
  );

  xTaskCreatePinnedToCore(
    writeSerial,
    "writeSerial",   //Task ID
    2048,               //Stack Size
    NULL,
    1,
    &writeSerial_h,   //TASK HANDLE
    1
  );
  vTaskResume(serialListen_h);
  vTaskDelete(NULL);    //Deletes the loop and setup function.

}

void loop(){
  }
