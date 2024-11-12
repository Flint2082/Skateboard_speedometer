#include "main.h"
#include "adc.h"
#include "driver/adc.h" // Include the header file that defines adc_channel_t


static adc_channel_t channel[1] = {ADC_CHANNEL_6};

static TaskHandle_t s_task_handle;
static const char *TAG = "EXAMPLE"; 

// Callback function for the ADC continuous mode driver
static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t adc_handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    BaseType_t mustYield = pdFALSE;
    //Notify that ADC continuous driver has done enough number of conversions
    vTaskNotifyGiveFromISR(s_task_handle, &mustYield);

    return (mustYield == pdTRUE);
}

void app_main(void)
{   
    esp_err_t ret; // var for error checking
    uint32_t ret_num = 0; // 
    uint8_t result[READ_LEN] = {0}; // array to store the result of the ADC conversion
    memset(result, 0xcc, READ_LEN); // fill the array with 0xcc

    s_task_handle = xTaskGetCurrentTaskHandle();

    adc_continuous_handle_t adc_handle = NULL;
    continuous_adc_init(channel, sizeof(channel) / sizeof(adc_channel_t), &adc_handle, READ_SPEED);

    adc_continuous_evt_cbs_t cbs = { // callback functions for the ADC continuous mode driver
        .on_conv_done = s_conv_done_cb,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(adc_handle, &cbs, NULL)); 
    ESP_ERROR_CHECK(adc_continuous_start(adc_handle)); // start the ADC continuous mode driver

    while (1) {

        /**
         * This is to show you the way to use the ADC continuous mode driver event callback.
         * This `ulTaskNotifyTake` will block when the data processing in the task is fast.
         * However in this example, the data processing (print) is slow, so you barely block here.
         *
         * Without using this event callback (to notify this task), you can still just call
         * `adc_continuous_read()` here in a loop, with/without a certain block timeout.
         */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // block the task until the notification is received                

        char unit[] = ADC_UNIT_STR(ADC_UNIT);
        
        while (1) {
            uint32_t average = 0;
            ret = adc_continuous_read(adc_handle, result, READ_LEN, &ret_num, 0);
            if (ret == ESP_OK) {
                // ESP_LOGI("TASK", "ret is %x, ret_num is %"PRIu32" bytes", ret, ret_num);
                for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
                    adc_digi_output_data_t *p = (adc_digi_output_data_t*)&result[i];
                    uint32_t chan_num = ADC_GET_CHANNEL(p);
                    uint32_t data = ADC_GET_DATA(p);
                    
                    /* Check the channel number validation, the data is invalid if the channel num exceed the maximum channel */
                    if (chan_num < SOC_ADC_CHANNEL_NUM(ADC_UNIT)) {
                        // ESP_LOGI(TAG, "Unit: %s, Channel: %"PRIu32", Value: %"PRIx32, unit, chan_num, data);
                        // // ESP_LOGI(TAG, "ADC: %"PRIu32, data);
                        average = average + data;
                        // if(i == 0) {
                        //     // ESP_LOGI(TAG, "ADC: %"PRIu32, data);
                        //     printf("%li \n", data);
                        // }
                        
                    } else {
                        ESP_LOGW(TAG, "Invalid data [%s_%"PRIu32"_%"PRIx32"]", unit, chan_num, data);
                    }
                    
                }

                printf("%li \n", (average/(ret_num/SOC_ADC_DIGI_RESULT_BYTES)));
                
                /**
                 * Because printing is slow, so every time you call `ulTaskNotifyTake`, it will immediately return.
                 * To avoid a task watchdog timeout, add a delay here. When you replace the way you process the data,
                 * usually you don't need this delay (as this task will block for a while).
                 */
                vTaskDelay(10 / portTICK_PERIOD_MS); // was 1 
            } else if (ret == ESP_ERR_TIMEOUT) {
                //We try to read `READ_LEN` until API returns timeout, which means there's no available data
                break;
            }
        }
    }

    ESP_ERROR_CHECK(adc_continuous_stop(adc_handle));
    ESP_ERROR_CHECK(adc_continuous_deinit(adc_handle));
}