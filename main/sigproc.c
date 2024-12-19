#include "sigproc.h"



#define TAG "SIGPROC"

float freq_analyzer(uint16_t* input_buf, uint16_t buf_len)
{

    float sum = 0;
    uint16_t average = 0;
    uint16_t zero_crossing = 0;
    float avg_period = 0;
    uint16_t prev_cross = 0;
    float freq = 0;
    bool schmitt_trigger = SCHMITT_LOW;   // schmitt trigger for zero crossing detection
    bool schmitt_trigger_prev = SCHMITT_LOW;   

    // average filter and calculate mean
    for (int i = 0; i < buf_len - KERNEL_SIZE; i++)
    {
        sum += input_buf[i];
        for (int j = 1; j < KERNEL_SIZE; j++)
        {
            input_buf[i] += input_buf[i + j];
        }
        input_buf[i] /= KERNEL_SIZE;
    }
    average = (uint16_t)round(sum / buf_len);
    // printf("average: %u\t", average);

    // zero crossing detection
    for(int i = 1; i < buf_len - KERNEL_SIZE; i++)
    {
        if(schmitt_trigger == SCHMITT_LOW && input_buf[i] > average + SCHMITT_TRIGGER) // schmitt trigger for zero crossing detection
        {
            schmitt_trigger = SCHMITT_HIGH;
        }
        else if(schmitt_trigger == SCHMITT_HIGH && input_buf[i] < average - SCHMITT_TRIGGER)
        {
            schmitt_trigger = SCHMITT_LOW;
        }
        
        if(schmitt_trigger == SCHMITT_LOW && schmitt_trigger_prev == SCHMITT_HIGH) // check for zero crossing
        {
            zero_crossing++;
            if(zero_crossing > 1)// calculate the period
            { 
                avg_period += i - prev_cross;
                // printf("zero crossing at: %d with a period of: %d\n", i, i - prev_cross);
            }    
            prev_cross = i;
        }
        schmitt_trigger_prev = schmitt_trigger;
    }
    if (zero_crossing > 1) {
        avg_period /= zero_crossing - 1; // calculate the average period (minus the first zero crossing)
        freq = (float)READ_SPEED / ((float)avg_period * ADC_DOWN_SCALE); // calculate the frequency
    } else {
        avg_period = 0; // handle the case where zero_crossing is 1 or less
        freq = 0;
    }
    
    

    // printf("zero crossing: %u\t", zero_crossing); // print the zero crossing
    // printf("avg period: %f\t", avg_period); // print the average period
    // printf("freq from period: %f\n", freq); // print the frequency

    return freq;
}

void analog_to_freq_conversion_task(void* parameters)
{
    uint16_t* input_buf = (uint16_t*)malloc(sizeof(uint16_t) * ADC_BUF_SIZE); // allocate memory for the input buffer
    if (input_buf == NULL) ESP_LOGE(TAG, "convolution buffer memory allocation failed");
    memset(input_buf, 0, sizeof(uint16_t) * ADC_BUF_SIZE); // fill the buffer with 0

    data_t speed_data = { // create a data structure to store the speed data
                .speed = 0, // store the speed
                .timestamp = 0, // store the timestamp
                .sensor_label = REFLECTIVE_SENSOR, // store the sensor label
            };
    
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
                adc_data_t* queue_buf = (adc_data_t*)malloc(sizeof(adc_data_t)); // allocate memory for the queue buffer
                if (queue_buf == NULL) ESP_LOGE(TAG, "queue buffer memory allocation failed");

                if (xQueueReceive(adc_queue_handle, queue_buf, 0) != pdTRUE) ESP_LOGE(TAG, "Failed to receive data from the queue at frame %d", i);
                memcpy(input_buf + (ADC_BUF_SIZE - (DS_FRAME_SIZE*FRAMES_PER_CONVERSION)) + (DS_FRAME_SIZE * i), queue_buf->adc_ds_vals, sizeof(uint16_t) * DS_FRAME_SIZE); // copy the data to the input buffer
            
                speed_data.timestamp = queue_buf->timestamp; // store the timestamp

                free(queue_buf); // free the memory allocated for the queue buffer
            }
            
            // convertion happens here
            float frequency = freq_analyzer(input_buf, ADC_BUF_SIZE); // convert the analog signal to frequency
            float speed = frequency * WHEEL_DIAMETER * M_PI * 3.6; // calculate the speed    
        
            
            speed_data.speed = speed; // store the speed
            
            printf("label: %d\t timestamp: %f\t speed: %f\n", speed_data.sensor_label, speed_data.timestamp ,speed_data.speed);


            xQueueSend(data_log_queue_handle, &speed_data, 0); // send the data to the data logging task
        }
        else
        {
            vTaskDelay(10 / portTICK_PERIOD_MS); // delay the task for 1ms
        }
    }
}
