#include <stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"

#include <FreeRTOS.h>
#include "task.h"
#include <pico/time.h>

#include "busy.h"

#define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 4UL)
#define HIGH_PRIORITY_TASK_PRIORITY (tskIDLE_PRIORITY + 3UL)
#define MEDIUM_PRIORITY_TASK_PRIORITY (tskIDLE_PRIORITY + 2UL)
#define LOW_PRIORITY_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define PRIORITY_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

void setUp(void) {}
void tearDown(void) {}

void test(TaskFunction_t t1_func, TaskFunction_t t2_func, uint64_t *t1_time, uint64_t *t2_time, uint64_t t1_delay_ms, uint64_t t2_delay_ms, BaseType_t t1_priority, BaseType_t t2_priority) {
    
    TaskHandle_t t1, t2;
    //TickType_t start_ticks = xTaskGetTickCount();
    //uint64_t start_count = portGET_RUN_TIME_COUNTER_VALUE();

    xTaskCreate(t1_func, "task_1", PRIORITY_TASK_STACK_SIZE, NULL, t1_priority, &t1);
    vTaskDelay(pdMS_TO_TICKS(t1_delay_ms));

    xTaskCreate(t2_func, "t2", PRIORITY_TASK_STACK_SIZE, NULL, t2_priority, &t2);
    vTaskDelay(pdMS_TO_TICKS(t2_delay_ms));

    TaskStatus_t t1_status, t2_status;

    vTaskGetInfo(t1, &t1_status, pdTRUE, eInvalid);
    vTaskGetInfo(t2, &t2_status, pdTRUE, eInvalid);

    *t1_time = t1_status.ulRunTimeCounter;
    *t2_time = t2_status.ulRunTimeCounter;


    printf("Task 1 Runtime: %llu\n", *t1_time);
    printf("Task 2 Runtime: %llu\n", *t2_time);

    vTaskDelete(t1);
    vTaskDelete(t2);
}


void test_both_busy_busy(void)
{
    uint64_t t1_time = 0;
    uint64_t t2_time = 0;

    uint64_t t1_delay_ms = 0;
    uint64_t t2_delay_ms = 0;

    test(busy_busy, busy_busy, &t1_time, &t2_time, t1_delay_ms, t2_delay_ms, MEDIUM_PRIORITY_TASK_PRIORITY, MEDIUM_PRIORITY_TASK_PRIORITY);

    uint64_t totalTime = t1_time + t2_time;

    TEST_ASSERT(totalTime - t1_time <= 0.6*totalTime || totalTime - t1_time >= .4*totalTime);

}

void test_both_busy_yield(void)
{
    uint64_t t1_time = 0;
    uint64_t t2_time = 0;

    uint64_t t1_delay_ms = 0;
    uint64_t t2_delay_ms = 0;

    uint64_t totalTime = t1_time + t2_time;

    test(busy_yield, busy_yield, &t1_time, &t2_time, t1_delay_ms, t2_delay_ms, MEDIUM_PRIORITY_TASK_PRIORITY, MEDIUM_PRIORITY_TASK_PRIORITY);
    TEST_ASSERT(totalTime - t1_time <= 0.6*totalTime || totalTime - t1_time >= .4*totalTime);

}

void test_busy_busyAndBusy_yield(void)
{
    uint64_t t1_time = 0;
    uint64_t t2_time = 0;

    uint64_t t1_delay_ms = 0;
    uint64_t t2_delay_ms = 0;

    test(busy_busy, busy_yield, &t1_time, &t2_time, t1_delay_ms, t2_delay_ms, MEDIUM_PRIORITY_TASK_PRIORITY, MEDIUM_PRIORITY_TASK_PRIORITY);
    TEST_ASSERT(t1_time == t2_time);
}

void test_both_busy_busy_high_prio_first(void)
{
    // t1 has the higher priority
    uint64_t t1_time = 0;
    uint64_t t2_time = 0;

    uint64_t t1_delay_ms = 0;
    uint64_t t2_delay_ms = 50;

    test(busy_busy, busy_busy, &t1_time, &t2_time, t1_delay_ms, t2_delay_ms, MEDIUM_PRIORITY_TASK_PRIORITY, LOW_PRIORITY_TASK_PRIORITY);
    TEST_ASSERT(t2_time == 0);
}

void test_both_busy_busy_low_prio_first(void)
{
    // t1 has the higher priority
    uint64_t t1_time = 0;
    uint64_t t2_time = 0;

    uint64_t t1_delay_ms = 50;
    uint64_t t2_delay_ms = 0;

    test(busy_busy, busy_busy, &t1_time, &t2_time, t1_delay_ms, t2_delay_ms, MEDIUM_PRIORITY_TASK_PRIORITY, LOW_PRIORITY_TASK_PRIORITY);
    TEST_ASSERT(t2_time < t1_time);
}

void test_both_busy_yield_diff_prio(void)
{
    // t1 has the higher priority
    uint64_t t1_time = 0;
    uint64_t t2_time = 0;

    uint64_t t1_delay_ms = 0;
    uint64_t t2_delay_ms = 0;

    test(busy_yield, busy_yield, &t1_time, &t2_time, t1_delay_ms, t2_delay_ms, MEDIUM_PRIORITY_TASK_PRIORITY, LOW_PRIORITY_TASK_PRIORITY);
    TEST_ASSERT(t2_time == 0);
}

void testSupervisor(void *pvParams)
{
    vTaskDelay(pdMS_TO_TICKS(5000));
    printf("Start tests\n");
    UNITY_BEGIN(); 

    RUN_TEST(test_both_busy_busy);
    RUN_TEST(test_both_busy_yield);
    RUN_TEST(test_busy_busyAndBusy_yield);
    RUN_TEST(test_both_busy_busy_high_prio_first);
    RUN_TEST(test_both_busy_busy_low_prio_first);
    RUN_TEST(test_both_busy_yield_diff_prio);

    vTaskDelay(pdMS_TO_TICKS(5000));
    
    int result = UNITY_END();
    printf("Unity result = %d\n", result);
    vTaskDelete(NULL);
    return;

}

int main (void)
{
    stdio_init_all();

    xTaskCreate(testSupervisor, "supervisor thread", PRIORITY_TASK_STACK_SIZE, NULL, HIGH_PRIORITY_TASK_PRIORITY, NULL);
    vTaskStartScheduler();

    return 0;

}
