#include "sigproc.h"

#define KERNEL_SIZE 10  // kernel size for convolution


#define TAG "SIGPROC"

void analog_to_freq_conversion_task(void* parameters)
{
    // uint32_t sum = 0; // sum of the data buffer
    // uint16_t average = 0; // average of the data buffer
    // uint16_t zero_crossing = 0; // zero crossing count

    uint8_t kernel[KERNEL_SIZE]; // moving average kernel for convolution	    
    memset(kernel, 1, sizeof(uint8_t) * KERNEL_SIZE); // fill the kernel with 1

    int16_t* input_buf = (int16_t*)malloc(sizeof(uint16_t) * BUF_SIZE); // allocate memory for the input buffer
    if (input_buf == NULL) ESP_LOGE(TAG, "convolution buffer memory allocation failed");
    memset(input_buf, 0, sizeof(uint16_t) * BUF_SIZE); // fill the buffer with 0
    
    uint16_t* conv_buf = (uint16_t*)malloc(sizeof(uint16_t) * BUF_SIZE); // allocate memory for the convolution buffer
    if (conv_buf == NULL) ESP_LOGE(TAG, "convolution buffer memory allocation failed");
    memset(conv_buf, 0, sizeof(uint16_t) * BUF_SIZE); // fill the buffer with 0



    // for (int i = 0; i < buf_len - KERNEL_SIZE-1; i++)
    // {
    //     for(int j = 0; j < KERNEL_SIZE; j++)
    //     {
    //         conv_buf[i] += data_buf[i + j] * kernel[j];
    //     }  
    //     conv_buf[i] = conv_buf[i] >> 6; // divide by 64
    //     sum += conv_buf[i];
    // }
    // average = sum / (buf_len - KERNEL_SIZE - 1); // calculate the average of the data buffer
    // for(int i = 0; i < buf_len; i++)
    // {
    //     if(i != 0 && conv_buf[i] > average && conv_buf[i-1] < average) // check for zero crossing
    //     {
    //         zero_crossing++;
    //     }
    // }

    // memcpy(data_buf, conv_buf, sizeof(uint16_t) * buf_len); // copy the convolution result to the data buffer
    
    // if(gpio_get_level(BUTTON_GPIO))// read the GPIO level
    //     {
    //         for(int i = 0; i < BUF_SIZE; i++)
    //             {
    //                 printf("data: %u\n", data_buf[i]); // print the data buffer
    //             }
    //     } 
    // free(conv_buf); // free the convolution buffer
    
    while(1)
    {
        if(adc_queue_handle == NULL) // check if the queue handle is valid
        {
            ESP_LOGE(TAG, "ADC queue handle is NULL");
        }
        else if(uxQueueMessagesWaiting(adc_queue_handle) >= FRAMES_PER_CONVERSION) // check the number of messages in the queue
        {
            for(int i = 0; i < FRAMES_PER_CONVERSION; i++)
            {
                if(xQueueReceive(adc_queue_handle, input_buf + (FRAME_LEN*i), 0) != pdTRUE) // receive the data from the queue
                {
                    ESP_LOGE(TAG, "Failed to receive data from the queue at frame %d", i);
                }
            }
            
            // convertion happens here
            float sum = 0;
            for (int i = 0; i < BUF_SIZE; i++)
            {
                sum += input_buf[i];
            }
            printf("average: %f\n", sum/BUF_SIZE);
            
            
        }
        else
        {
            vTaskDelay(10 / portTICK_PERIOD_MS); // delay the task for 1ms
        }
    }
}
