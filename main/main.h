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
#include "driver/gptimer.h"


// #define DEBUG 


// Real world values
#define WHEEL_DIAMETER 0.078f // Wheel diameter in meters

// Defines
#define BUTTON_GPIO 2 // GPIO pin for the button

// ADC defines
#define ADC_SPEED_COMPENSATION      1.2206 // Because of a bug in the ESP32 ADC driver (only in the esp32-wroom)
#define READ_SPEED                  SOC_ADC_SAMPLE_FREQ_THRES_LOW //SOC_ADC_SAMPLE_FREQ_THRES_LOW // 20kHz

#define CHANNEL_NUM                 1
#define ADC_CHANNEL                 ADC_CHANNEL_6
#define POOL_SIZE                   (FRAME_SIZE * FRAMES_PER_POOL)
#define FRAMES_PER_POOL             FRAMES_PER_CONVERSION
#define FRAME_SIZE                  2048 // was 2048
#define FRAMES_PER_CONVERSION       4
#define ADC_BUFFER_MULTIPLIER       2
#define ADC_DOWN_SCALE              8 // has to be a power of 2
#define DS_FRAME_SIZE               (FRAME_SIZE / ADC_DOWN_SCALE)
#define ADC_BUF_SIZE                ((FRAME_SIZE * FRAMES_PER_CONVERSION * ADC_BUFFER_MULTIPLIER) / ADC_DOWN_SCALE) 
#define ADC_CONV_PERIOD             ((float)(FRAME_SIZE * FRAMES_PER_CONVERSION) / READ_SPEED)

// Global variables
extern gptimer_handle_t glob_tim_handle; // global timer handle
extern TaskHandle_t adc_task_handle; // global task handle for the ADC task
extern TaskHandle_t conversion_task_handle; // global task handle for the conversion task
extern QueueHandle_t adc_queue_handle; // global queue handle for the ADC queue



// Task priorities
enum task_priority {
    IO_TASK_PRIORITY = 3,
    ADC_TASK_PRIORITY = 4,
    CONVERSION_TASK_PRIORITY = 5,
};


#endif // MAIN_H