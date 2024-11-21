#include "adc.h"

static TaskHandle_t adc_task_handle; // task handle for the ADC task
static const char *TAG = "ADC"; // tag for the ADC task




// #define ADC_UNIT ADC_UNIT_1
void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle)
{
    ESP_LOGI(TAG, "Start ADC init task");
    adc_continuous_handle_t handle = NULL;

    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = sizeof(uint16_t)*POOL_SIZE,
        .conv_frame_size = sizeof(uint16_t)*FRAME_LEN,
    };  
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle)); // create a new handle for the ADC continuous mode driver

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = READ_SPEED, 
        .conv_mode = ADC_CONV_MODE,
        .format = ADC_OUTPUT_TYPE,
    };

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0}; // memory alloc for the pattern config
    dig_cfg.pattern_num = channel_num;
    for (int i = 0; i < channel_num; i++) {
        adc_pattern[i].atten = ADC_ATTEN;
        adc_pattern[i].channel = channel[i] & 0x7;
        adc_pattern[i].unit = ADC_UNIT;
        adc_pattern[i].bit_width = ADC_BIT_WIDTH;

        ESP_LOGI(TAG, "adc_pattern[%d].atten is :%"PRIx8, i, adc_pattern[i].atten);
        ESP_LOGI(TAG, "adc_pattern[%d].channel is :%"PRIx8, i, adc_pattern[i].channel);
        ESP_LOGI(TAG, "adc_pattern[%d].unit is :%"PRIx8, i, adc_pattern[i].unit);
    }
    dig_cfg.adc_pattern = adc_pattern; // set the pattern config to the digital config
    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg)); // configure the ADC continuous mode driver

    *out_handle = handle; // return the handle
}