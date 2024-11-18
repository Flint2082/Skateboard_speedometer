#include "sigproc.h"

#define KERNEL {1, 1, 1, 1, 1}

#define TAG "SIGPROC"

uint16_t freq_reader(uint32_t* data_buf, uint16_t buf_len)
{
    float kernel[5] = KERNEL;
    uint32_t* conv_buf = (uint32_t*)malloc(sizeof(uint32_t) * (buf_len + 4));
    if (conv_buf == NULL)
    {
        // ESP_LOGE(TAG, "Memory allocation failed");
        return 0;
    }
    for(int i = 0; i < buf_len + 4; i++)
    {
        conv_buf[i] = 0;
    }


    for (int i = 0; i < buf_len - 4; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            conv_buf[i] += data_buf[i + j] * kernel[j];
        }   
    }

    for(int i = 0; i < buf_len; i++)
    {
        printf("%lu\n", conv_buf[i]);
    }

    // vTaskDelay(1000 / portTICK_PERIOD_MS);

    
    return 0;
}
