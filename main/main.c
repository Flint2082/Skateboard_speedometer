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

    ESP_LOGI(TAG, "Memory allocation for frame buffer");
    adc_digi_output_data_t* result = (adc_digi_output_data_t*)malloc(sizeof(adc_digi_output_data_t) * FRAME_LEN); // allocate memory for the frame buffer
    if(result == NULL) ESP_LOGE(TAG, "Failed to allocate frame buffer memory"); 
    memset(result, 0, sizeof(adc_digi_output_data_t) *FRAME_LEN); // fill the array with 0

    ESP_LOGI(TAG, "Memory allocation for conv buffer");
    uint16_t* adc_buffer = (uint16_t*)malloc(sizeof(uint16_t) * BUF_SIZE); // allocate memory for the conv buffer
    if(adc_buffer == NULL) ESP_LOGE(TAG, "Failed to allocate conv buffer memory"); 
    memset(adc_buffer, 0, sizeof(uint16_t) * BUF_SIZE); // fill the buffer with 0

    
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
        #ifdef DEBUG 
            (TAG, "Waiting for ADC data ready");
        #endif
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // block the task until the notification is received    
        #ifdef DEBUG            
    	    ESP_LOGI(TAG, "ADC data ready");
        #endif
        char unit[] = ADC_UNIT_STR(ADC_UNIT);
        
        while (1) {
            uint32_t average = 0;
            ret = adc_continuous_read(adc_handle, result, sizeof(adc_digi_output_data_t)*FRAME_LEN, &ret_num, 0);
            #ifdef DEBUG
                ESP_LOGI(TAG, "Read %lu samples", ret_num);
            #endif
            if (ret == ESP_OK) {
                for (int i = 0; i < FRAME_LEN; i++) {
                    adc_buffer[framecount*FRAME_LEN + i] = adc_buffer[(FRAMES_PER_CONVERSION + framecount)*FRAME_LEN + i]; // shift one frame in the buffer by 4 frames
                    adc_buffer[(FRAMES_PER_CONVERSION + framecount)*FRAME_LEN + i] = result[i].type1.data; // store the result of the ADC conversion in the buffer
                }          
                framecount++; // increment the frame count
                // printf("framecount: %u\n", framecount);
                if(framecount == FRAMES_PER_CONVERSION) // every 4 frames
                {
                    for(int i = BUF_SIZE/2; i < BUF_SIZE; i = i + 100)
                    {
                        printf("data: %u\tframecount: %u\n", adc_buffer[i], framecount);
                    }
                    // uint16_t freq = freq_reader(adc_buffer, BUF_SIZE); // call the freq_reader function
                    framecount = 0; // reset the frame count
                }
                vTaskDelay(30 / portTICK_PERIOD_MS); // was 1 
            } else if (ret == ESP_ERR_TIMEOUT) {
                //We try to read `FRAME_LEN` until API returns timeout, which means there's no available data
                break;
            }
        }
    }

    ESP_ERROR_CHECK(adc_continuous_stop(adc_handle));
    ESP_ERROR_CHECK(adc_continuous_deinit(adc_handle));
}