#include <Arduino.h>
#include "device.h"

void taskTrigger(void* arg);
void taskProcess(void* arg);
SemaphoreHandle_t smDataUpdate;
volatile int nCounter=0;

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  smDataUpdate = xSemaphoreCreateBinary();
  xTaskCreatePinnedToCore(taskTrigger, "taskProducer", configMINIMAL_STACK_SIZE+1024, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(taskProcess, "taskConsumer", configMINIMAL_STACK_SIZE+1024, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelay(0);
}

void taskTrigger(void* arg)
{
    while (1)
    {    
        digitalWrite(LED_RED, LED_ON);
        nCounter++;
        printf("Trigger: %d\n", nCounter);
        fflush(stdout);
        vTaskDelay(10/portTICK_RATE_MS);
        digitalWrite(LED_RED, LED_OFF);
        xSemaphoreGive(smDataUpdate);
        vTaskDelay(2000/portTICK_RATE_MS);
    }
}

void taskProcess(void* arg)
{
    while (1)
    {
      xSemaphoreTake(smDataUpdate, portMAX_DELAY);
      digitalWrite(LED_GREEN, LED_ON);        
      vTaskDelay(100/portTICK_RATE_MS);
      printf("Proces receive: %d\n", nCounter);
      fflush(stdout);
      digitalWrite(LED_GREEN, LED_OFF);
    }
}
