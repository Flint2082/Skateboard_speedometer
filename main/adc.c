#include "adc.h"




// Callback function for the ADC continuous mode driver
static bool IRAM_ATTR adc_conversion_done_callback(adc_continuous_handle_t adc_handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    BaseType_t mustYield = pdFALSE;
    //Notify that ADC continuous driver has done enough number of conversions
    vTaskNotifyGiveFromISR(adc_task_handle, &mustYield);

    return (mustYield == pdTRUE);
}


// #define ADC_UNIT ADC_UNIT_1
void adc_read_task(void* parameters)
{
    static const char* const TAG = "ADC"; // tag for the ADC task
    ESP_LOGI(TAG, "Start ADC init task"); // log the start of the ADC task

    // local variables for the ADC task
    esp_err_t ret; // var for error checking
    uint8_t framecount = 0; // var to store the frame count
    uint32_t ret_num = 0; //  var to store the return number

    ESP_LOGI(TAG, "Memory allocation for frame buffer");
    adc_digi_output_data_t* result = (adc_digi_output_data_t*)calloc(FRAME_LEN, sizeof(adc_digi_output_data_t)); // allocate memory for the frame buffer
    if(result == NULL) ESP_LOGE(TAG, "Failed to allocate frame buffer memory"); 

    uint16_t adc_vals[FRAME_LEN]; // array to store the ADC values
    
    // ESP_LOGI(TAG, "Memory allocation for adc buffer");
    // uint16_t* adc_buffer = (uint16_t*)malloc(sizeof(uint16_t) * BUF_SIZE); // allocate memory for the adc buffer
    // if(adc_buffer == NULL) ESP_LOGE(TAG, "Failed to allocate adc buffer memory"); 
    // memset(adc_buffer, 0, sizeof(uint16_t) * BUF_SIZE); // fill the buffer with 0

    adc_continuous_handle_t adc_handle;

    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = sizeof(uint16_t) * POOL_SIZE,
        .conv_frame_size = sizeof(uint16_t) * FRAME_LEN,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &adc_handle)); // create a new handle for the ADC continuous mode driver

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = READ_SPEED,       // Sampling frequency
        .conv_mode      = ADC_CONV_MODE,    // Conversion mode
        .format         = ADC_OUTPUT_TYPE,  // Output format
        .pattern_num    = CHANNEL_NUM,      // Number of patterns
    };

    adc_digi_pattern_config_t adc_pattern = {
        .atten      = ADC_ATTEN,        // attenuation
        .channel    = ADC_CHANNEL & 0x7, // channel number
        .unit       = ADC_UNIT,         // ADC unit
        .bit_width  = ADC_BIT_WIDTH,    // bit width
    };


    ESP_LOGI(TAG, "adc_pattern.atten is :%"PRIx8, adc_pattern.atten);
    ESP_LOGI(TAG, "adc_pattern.channel is :%"PRIx8, adc_pattern.channel);
    ESP_LOGI(TAG, "adc_pattern.unit is :%"PRIx8, adc_pattern.unit);
    ESP_LOGI(TAG, "adc_pattern.bit_width is :%"PRIx8, adc_pattern.bit_width);


    dig_cfg.adc_pattern = &adc_pattern; // set the pattern config to the digital config
    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &dig_cfg)); // configure the ADC continuous mode driver

    

    adc_continuous_evt_cbs_t cbs = { // callback functions for the ADC continuous mode driver
        .on_conv_done = adc_conversion_done_callback
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(adc_handle, &cbs, NULL)); 
    ESP_ERROR_CHECK(adc_continuous_start(adc_handle)); // start the ADC continuous mode driver

    while(1)
    {
        #ifdef DEBUG 
            ESP_LOGI(TAG, "Waiting for ADC data ready");
        #endif
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // block the task until the notification is received    
        #ifdef DEBUG            
    	    ESP_LOGI(TAG, "ADC data ready");
        #endif 


        ret = adc_continuous_read(adc_handle, (uint8_t*)result, sizeof(uint16_t)*FRAME_LEN, &ret_num, 0);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to read ADC data");
        }
        else
        {
            #ifdef DEBUG
                ESP_LOGI(TAG, "ADC data read successfully");
            #endif

            for (int i = 0; i < FRAME_LEN; i++) 
            {
                // add possible downscaling here
                adc_vals[i] = result[i].type1.data; // store the result of the ADC conversion in the buffer

                // adc_buffer[((FRAMES_PER_CONVERSION + framecount)*FRAME_LEN) + i] = result[i].type1.data; // store the result of the ADC conversion in the buffer
            }          

            if(xQueueSend(adc_queue_handle, adc_vals, 0) != pdTRUE) // send the ADC value to the queue
            {
                ESP_LOGE(TAG, "Failed to send ADC data to the queue");
            }



            // if(framecount == FRAMES_PER_CONVERSION) // every 4 frames
            // {
            //     // freq = freq_reader(adc_buffer, BUF_SIZE); // call the freq_reader function
            //     // printf("zero crossing: %u\n", freq); // print the zero crossing count
                
            //     framecount = 0; // reset the frame count
            //     memcpy(adc_buffer, &adc_buffer[FRAME_LEN*FRAMES_PER_CONVERSION], sizeof(uint16_t) * FRAME_LEN*FRAMES_PER_CONVERSION); // shift the buffer by 4 frames
            // }

           

            // framecount++;
            // if(framecount == FRAMES_PER_POOL)
            // {
            //     framecount = 0;
                
                
            // }
        }
    }   
}