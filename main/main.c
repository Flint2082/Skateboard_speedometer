#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_adc/adc_continuous.h"
#include "esp_mac.h"


void app_main(void)
{
    
    adc_continuous_handle_cfg_t adc_init_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = 100,
    };
    // ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_init_config));
    adc_continuous_config_t adc_config = {
        .pattern_num = 1,
        // .adc_pattern = ?,
        .sample_freq_hz = 1000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1
    };

    adc_digi_pattern_config_t adc_digi_config = {
        .atten = ADC_ATTEN_DB_0,
        .channel = ADC_CHANNEL_0, // is this the correct channel?
        .unit = ADC_UNIT_1, // what does this mean?
        .bit_width = ADC_BITWIDTH_10
    };

    // adc_continuous_config();

    while(true)
    {
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    //     printf("ADC: %i", ADC_val); // print ADC value
    }
    // ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config));

}
