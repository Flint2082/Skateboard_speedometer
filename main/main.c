#include "main.h"
#include "adc.h"
#include "sigproc.h"



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
    ESP_LOGI(TAG, "Start main task");
    esp_err_t ret; // var for error checking
    uint8_t framecount = 0; // var to store the frame count
    uint32_t ret_num = 0; // 
    uint8_t result[FRAME_LEN] = {0}; // array to store the result of the ADC conversion
    memset(result, 0xcc, FRAME_LEN); // fill the array with 0xcc
    uint32_t adc_buffer[BUF_SIZE] = {0}; // array to store the result of the ADC conversion for 8 frames

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
        ESP_LOGI(TAG, "Waiting for ADC data ready");
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // block the task until the notification is received                
    	ESP_LOGI(TAG, "ADC data ready");
        char unit[] = ADC_UNIT_STR(ADC_UNIT);
        
        while (1) {
            uint32_t average = 0;
            ret = adc_continuous_read(adc_handle, result, FRAME_LEN, &ret_num, 0);
            if (ret == ESP_OK) {
                for (int i = 0; i < FRAME_LEN; i++) {
                    adc_buffer[framecount*FRAME_LEN + i] = adc_buffer[FRAMES_PER_CONVERSION + framecount*FRAME_LEN + i]; // shift one frame in the buffer by 4 frames
                    adc_buffer[FRAMES_PER_CONVERSION + framecount*FRAME_LEN + i] = result[i]; // store the result of the ADC conversion in the buffer
                }              
                framecount++; // increment the frame count
                if(framecount == FRAMES_PER_CONVERSION) // if the frame count is equal to 4
                {
                    // uint16_t freq = freq_reader(adc_buffer, BUF_SIZE); // call the freq_reader function
                    printf("final buf value: %lu\n", adc_buffer[BUF_SIZE - 1]); // print the final value of the buffer
                    framecount = 0; // reset the frame count
                }
                vTaskDelay(10 / portTICK_PERIOD_MS); // was 1 
            } else if (ret == ESP_ERR_TIMEOUT) {
                //We try to read `FRAME_LEN` until API returns timeout, which means there's no available data
                break;
            }
        }
    }

    ESP_ERROR_CHECK(adc_continuous_stop(adc_handle));
    ESP_ERROR_CHECK(adc_continuous_deinit(adc_handle));
}