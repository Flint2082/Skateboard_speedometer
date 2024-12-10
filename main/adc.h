#ifndef ADC_H
#define ADC_H

#include <string.h>
#include <stdio.h>

#include "main.h"

// #include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_continuous.h"
#include "esp_mac.h"



#define ADC_UNIT                    ADC_UNIT_1
#define _ADC_UNIT_STR(unit)         #unit
#define ADC_UNIT_STR(unit)          _ADC_UNIT_STR(unit)
#define ADC_CONV_MODE               ADC_CONV_SINGLE_UNIT_1
#define ADC_ATTEN                   ADC_ATTEN_DB_11
#define ADC_BIT_WIDTH               SOC_ADC_DIGI_MAX_BITWIDTH

#define ADC_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE1
#define ADC_GET_CHANNEL(p_data)     ((p_data)->type1.channel)
#define ADC_GET_DATA(p_data)        ((p_data)->type1.data)

#define CHANNEL_NUM                 1
#define ADC_CHANNEL                 ADC_CHANNEL_6
#define READ_SPEED                  SOC_ADC_SAMPLE_FREQ_THRES_LOW // 20kHz
#define POOL_SIZE                   2048
#define FRAMES_PER_POOL             4
#define FRAME_LEN                   (POOL_SIZE / FRAMES_PER_POOL)
#define FRAMES_PER_CONVERSION       4
#define ADC_BUFFER_MULTIPLIER       2
#define BUF_SIZE                    (FRAME_LEN * FRAMES_PER_CONVERSION * ADC_BUFFER_MULTIPLIER)



void adc_read_task(void* parameters); 


#endif // ADC_H
