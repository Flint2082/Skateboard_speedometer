#include "sigproc.h"

#define KERNEL_SIZE 10  // kernel size for convolution


#define TAG "SIGPROC"

uint16_t freq_analyzer(uint16_t* input_buf, uint16_t buf_len)
{
    float sum = 0;
    uint16_t average = 0;
    uint16_t zero_crossing = 0;
    
    for (int i = 0; i < buf_len; i++)
    {
        sum += input_buf[i];
    }
    average = (uint16_t)round(sum / buf_len);
    // printf("average: %u\t", average);
    
    // zero crossing detection
    for(int i = 0; i < buf_len; i++)
    {
        if(i != 0 && input_buf[i] > average && input_buf[i-1] < average) // check for zero crossing
        {
            zero_crossing++;
        }
    }
    float freq = (float)zero_crossing / (ADC_CONV_PERIOD*ADC_BUFFER_MULTIPLIER); // calculate the frequency

    uint32_t time_us = 0;
    uint32_t resolution = 1;
    gptimer_get_raw_count(glob_tim_handle, &time_us); // get the timer count
    gptimer_get_resolution(glob_tim_handle, &resolution); // get the timer resolution

    printf("zero crossing: %u\t", zero_crossing); // print the zero crossing
    printf("period expected: %f\t", ADC_CONV_PERIOD); // print the period
    printf("period actual: %f\t", (float)time_us/(float)resolution); // print the period
    printf("freq: %u\n", (uint16_t)round(freq)); // print the frequency
    // uint16_t buffer_debug[buf_len/64];
    // for(int i = 0; i < buf_len/64; i++)
    // {
    //     buffer_debug[i] = input_buf[i*64];
    // }


    // if(gpio_get_level(BUTTON_GPIO) > 0)
    // {
    //     for(int i = 0; i < buf_len; i+= 100)
    //     {
            
    //         // printf("%u\n", input_buf[i]);
    //         // vTaskDelay(1 / portTICK_PERIOD_MS);
    //     }
    // }
    // buffer_debug[1000] = buffer_debug[1001];

    gptimer_set_raw_count(glob_tim_handle, 0); // reset the timer

    return zero_crossing;
}

void analog_to_freq_conversion_task(void* parameters)
{
    uint8_t kernel[KERNEL_SIZE]; // moving average kernel for convolution	    
    memset(kernel, 1, sizeof(uint8_t) * KERNEL_SIZE); // fill the kernel with 1

    uint16_t* input_buf = (uint16_t*)malloc(sizeof(uint16_t) * ADC_BUF_SIZE); // allocate memory for the input buffer
    if (input_buf == NULL) ESP_LOGE(TAG, "convolution buffer memory allocation failed");
    memset(input_buf, 0, sizeof(uint16_t) * ADC_BUF_SIZE); // fill the buffer with 0
    
    uint16_t* conv_buf = (uint16_t*)malloc(sizeof(uint16_t) * ADC_BUF_SIZE); // allocate memory for the convolution buffer
    if (conv_buf == NULL) ESP_LOGE(TAG, "convolution buffer memory allocation failed");
    memset(conv_buf, 0, sizeof(uint16_t) * ADC_BUF_SIZE); // fill the buffer with 0

    
    while(1)
    {
        if(adc_queue_handle == NULL) // check if the queue handle is valid
        {
            ESP_LOGE(TAG, "ADC queue handle is NULL");
        }
        else if(uxQueueMessagesWaiting(adc_queue_handle) >= FRAMES_PER_CONVERSION) // check the number of messages in the queue
        {
            #if ADC_BUFFER_MULTIPLIER > 1
                memcpy(input_buf, input_buf + DS_FRAME_SIZE, sizeof(uint16_t) * (ADC_BUF_SIZE - DS_FRAME_SIZE)); // copy the convolution result to the input buffer
            #endif
            for (int i = 0; i < FRAMES_PER_CONVERSION; i++)
            {
                if (xQueueReceive(adc_queue_handle, input_buf + (ADC_BUF_SIZE - (DS_FRAME_SIZE*FRAMES_PER_CONVERSION)) + (DS_FRAME_SIZE * i), 0) != pdTRUE) // receive the data from the queue
                {
                    ESP_LOGE(TAG, "Failed to receive data from the queue at frame %d", i);
                }
            }
            
            // convertion happens here
            uint16_t zero_crossing = freq_analyzer(input_buf, ADC_BUF_SIZE); // convert the analog signal to frequency
        }
        else
        {
            vTaskDelay(10 / portTICK_PERIOD_MS); // delay the task for 1ms
        }
    }
}
