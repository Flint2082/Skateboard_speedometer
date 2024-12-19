#ifndef MAIN_H
#define MAIN_H

#include <string.h>
#include <stdio.h>
#include <math.h>

#include "adc.h"
#include "sigproc.h"
#include "dataLogging.h"    

#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_continuous.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"	


// #define DEBUG 


// Real world values
#define WHEEL_DIAMETER 0.078f // Wheel diameter in meters

// Defines
#define BUTTON_GPIO 2 // GPIO pin for the button
#define LED_GPIO 4 // GPIO pin for the LED  

// ADC defines
#define ADC_SPEED_COMPENSATION      1.2206 // Because of a bug in the ESP32 ADC driver (https://github.com/espressif/esp-idf/issues/10612)
#define READ_SPEED                  (20 * 1000) //SOC_ADC_SAMPLE_FREQ_THRES_LOW // 20kHz

#define CHANNEL_NUM                 1
#define ADC_CHANNEL                 ADC_CHANNEL_6
#define POOL_SIZE                   (FRAME_SIZE * FRAMES_PER_POOL)
#define FRAMES_PER_POOL             (FRAMES_PER_CONVERSION * 1)
#define FRAME_SIZE                  2048 // was 2048
#define FRAMES_PER_CONVERSION       4
#define ADC_BUFFER_MULTIPLIER       1
#define ADC_DOWN_SCALE              8 // has to be a power of 2
#define DS_FRAME_SIZE               (FRAME_SIZE / ADC_DOWN_SCALE)
#define ADC_BUF_SIZE                ((FRAME_SIZE * FRAMES_PER_CONVERSION * ADC_BUFFER_MULTIPLIER) / ADC_DOWN_SCALE) 
#define ADC_CONV_PERIOD             ((float)(FRAME_SIZE * FRAMES_PER_CONVERSION) / READ_SPEED)

#define TIMER_RESOLUTION            1 * 1000 * 1000 // 1MHz, 1 tick = 1us

// Global variables
extern gptimer_handle_t glob_tim_handle; // global timer handle
extern TaskHandle_t adc_task_handle; // global task handle for the ADC task
extern TaskHandle_t conversion_task_handle; // global task handle for the conversion task
extern TaskHandle_t data_log_task_handle; // global task handle for the IO task
extern QueueHandle_t adc_queue_handle; // global queue handle for the ADC queue
extern QueueHandle_t data_log_queue_handle; // global queue handle for the IO queue

extern bool record_state; // global variable to store the record state

// Task priorities
enum task_priority {
    DATA_LOG_TASK_PRIORITY = 3,
    CONVERSION_TASK_PRIORITY,
    ADC_TASK_PRIORITY,
};

typedef enum {
    REFLECTIVE_SENSOR,
} sensor_label_t;

typedef struct {
    uint16_t adc_ds_vals[DS_FRAME_SIZE];
    float timestamp;
} adc_data_t;

typedef struct {
    float speed;
    float timestamp;
    sensor_label_t sensor_label;
} data_t;	




#endif // MAIN_H