#ifndef MAIN_H
#define MAIN_H

#include <string.h>
#include <stdio.h>
#include <math.h>

#include "adc.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_continuous.h"

#define BUTTON_GPIO 2 // GPIO pin for the button

extern BaseType_t adc_task_handle; // global task handle for the ADC task

// #define DEBUG 

#endif // MAIN_H