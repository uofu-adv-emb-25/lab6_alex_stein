#include <stdio.h>
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

void Task_High(void *pvParams)
{
    vTaskDelay(pdMS_TO_TICKS(200));
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(50));
    xSemaphoreGive(xSemaphore);
    vTaskDelete(NULL);
}

void Task_Medium(void *pvParams)
{
    while(1)
    {
        // starve other tasks
    }
}

void Task_Low(void *pvParams)
{
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(50));
    xSemaphoreGive(xSemaphore);
    vTaskDelete(NULL);
}

void Task_Supervisor(void)
{
    xSemaphore = xSemaphoreCreateBinary()

    xTaskCreate(Task_Low, "low_priority_thread", PRIORITY_TASK_STACK_SIZE, NULL, LOW_PRIORITY_TASK_PRIORITY, NULL);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    xTaskCreate(Task_Medium, "medium_priority_thread", PRIORITY_TASK_STACK_SIZE, NULL, MEDIUM_PRIORITY_TASK_PRIORITY, NULL);
    xTaskCreate(Task_High, "high_priority_thread", PRIORITY_TASK_STACK_SIZE, NULL, HIGH_PRIORITY_TASK_PRIORITY, NULL);
}

int main() {
    stdio_init_all();
    sleep_ms(5000);

    const char *rtos_name;
    rtos_name = "FreeRTOS";
    TaskHandle_t task;
    xTaskCreate(Task_Supervisor, "supervisor_thread", MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &task);

    vTaskStartScheduler();  // Start FreeRTOS

    return 0;
}