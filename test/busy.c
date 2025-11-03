#include <stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include "math.h"

void busy_busy(void)
{
    for (int i = 0; ; i++);
}

void busy_yield(void)
{
    for (int i = 0; ; i++) {
        taskYIELD();
    }
}