#ifndef ADC_H
#define ADC_H

#include <string.h>
#include <stdio.h>

#include "sdkconfig.h"
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
#define ADC_ATTEN                   ADC_ATTEN_DB_0
#define ADC_BIT_WIDTH               SOC_ADC_DIGI_MAX_BITWIDTH

#define ADC_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE1
#define ADC_GET_CHANNEL(p_data)     ((p_data)->type1.channel)
#define ADC_GET_DATA(p_data)        ((p_data)->type1.data)


#define READ_SPEED                  SOC_ADC_SAMPLE_FREQ_THRES_LOW // 20kHz
#define POOL_SIZE                   1024
#define FRAMES_PER_POOL             2
#define READ_LEN                    (POOL_SIZE / FRAMES_PER_POOL)



void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle, uint16_t sample_freq ); 


#endif // ADC_H
