#include <stdio.h>
#include <math.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include "task.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"

#define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 4UL)
#define HIGH_PRIORITY_TASK_PRIORITY (tskIDLE_PRIORITY + 3UL)
#define MEDIUM_PRIORITY_TASK_PRIORITY (tskIDLE_PRIORITY + 2UL)
#define LOW_PRIORITY_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define PRIORITY_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

SemaphoreHandle_t xSemaphore;

void Task_High(void *pvParams)
{
    vTaskDelay(pdMS_TO_TICKS(200));
    printf("High priority taks attempting to take semaphore\n");

    if(xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)    
    {
        printf("High priority task has taken semaphore\n");
        xSemaphoreGive(xSemaphore);
        printf("High priority task released semaphore\n");
    }
    else
    {
        printf("High priority task unable to take semaphore\n");
    }
    vTaskDelete(NULL);
}

void Task_Medium(void *pvParams)
{
    printf("Medium task attempting to starve system\n");
    while(1)
    {
        { taskYIELD(); }
        // starve other tasks   
    }
}

void Task_Low(void *pvParams)
{

    if(xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)
    {
        printf("Low priority task was able to take semaphore\n");

         // do something not fun for a while
        volatile float res = 0;
        for (int i = 0; i < 100; i++) 
            for (int j = 0; j < 100; j++) 
                for (int k = 1; k < 100; k++) 
                    res = sqrt((float) i);

        printf("Low priority task releasing semaphore now\n");
        xSemaphoreGive(xSemaphore);
    }
    else
    {
        printf("Low priority task was unable to take semaphore\n");
    }
    vTaskDelete(NULL);
}

void Task_Supervisor(void *pvParams)
{
    xTaskCreate(Task_Low, "low_priority_thread", PRIORITY_TASK_STACK_SIZE, NULL, LOW_PRIORITY_TASK_PRIORITY, NULL);
    vTaskDelay(pdMS_TO_TICKS(10));
    xTaskCreate(Task_Medium, "medium_priority_thread", PRIORITY_TASK_STACK_SIZE, NULL, MEDIUM_PRIORITY_TASK_PRIORITY, NULL);
    vTaskDelay(pdMS_TO_TICKS(150));
    xTaskCreate(Task_High, "high_priority_thread", PRIORITY_TASK_STACK_SIZE, NULL, HIGH_PRIORITY_TASK_PRIORITY, NULL);

    vTaskDelete(NULL);

}

int main() {
    stdio_init_all();
    sleep_ms(5000);

    xSemaphore = xSemaphoreCreateMutex();
    configASSERT(xSemaphore);

    TaskHandle_t task;
    xTaskCreate(Task_Supervisor, "supervisor_thread", MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &task);
    vTaskStartScheduler(); 

    return 0;
}