#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"


#define TASK_PRIORITY     ( tskIDLE_PRIORITY + 2UL )


void Task1(void *pvParams)
{
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(50));
    xSemaphoreGive(xSemaphore);
    vTaskDelete(NULL);
}

void Task2(void *pvParams)
{
    while(1)
    {
        // do some work
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void Task3(void *pvParams)
{
    vTaskDelay(pdMS_TO_TICKS(200));
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(50));
    xSemaphoreGive(xSemaphore);
    vTaskDelete(NULL);
}

void Supervisor(void)
{
    xSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(xSemaphore);

    BaseType_t l1 = xTaskCreate(Task1, "Low Priority Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL)
    BaseType_t l2 = xTaskCreate(Task2, "Med Priority Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL)
    BaseType_t l3 = xTaskCreate(Task3, "High Priority Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL)

}
