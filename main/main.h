#ifndef MAIN_H
#define MAIN_H

#include <string.h>
#include <stdio.h>
#include <math.h>

#include "adc.h"
#include "sigproc.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_continuous.h"

#define BUTTON_GPIO 2 // GPIO pin for the button

extern TaskHandle_t adc_task_handle; // global task handle for the ADC task
extern TaskHandle_t conversion_task_handle; // global task handle for the conversion task
extern QueueHandle_t adc_queue_handle; // global queue handle for the ADC queue

// #define DEBUG 

enum task_priority {
    IO_TASK_PRIORITY = 3,
    CONVERSION_TASK_PRIORITY = 4,
    ADC_TASK_PRIORITY = 5,
};


#endif // MAIN_H