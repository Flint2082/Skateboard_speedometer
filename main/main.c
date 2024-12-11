#include "main.h"
#include "adc.h"
#include "sigproc.h"

static const char *TAG = "MAIN"; 

TaskHandle_t adc_task_handle;         // global task handle for the ADC task
TaskHandle_t conversion_task_handle;  // global task handle for the conversion task
TaskHandle_t IO_task_handle;          // global task handle for the IO task
QueueHandle_t adc_queue_handle;               // global queue handle for the ADC queue
QueueHandle_t IO_queue;                // global queue handle for the IO queue


void app_main(void)
{   
    adc_queue_handle = xQueueCreate(            FRAMES_PER_CONVERSION * 2,                 // queue length
                                                sizeof(uint16_t) * FRAME_SIZE / ADC_DOWN_SCALE);    // queue item size

    // Create the ADC task
    BaseType_t adc_task_status = xTaskCreate(
        adc_read_task,      // task function
        "adc_task",         // task name
        4096,               // stack depth
        NULL,               // task parameters
        ADC_TASK_PRIORITY,  // task priority
        &adc_task_handle    // pointer to store the task handle
    );

    // Check if the ADC task was created successfully
    if (adc_task_status != pdPASS) {
        // Handle error
    }

    // Create the conversion task
    BaseType_t conversion_task_status = xTaskCreate(
        analog_to_freq_conversion_task, // task function
        "conversion_task",              // task name
        4096,                           // stack depth
        NULL,                           // task parameters
        CONVERSION_TASK_PRIORITY,       // task priority
        &conversion_task_handle         // pointer to store the task handle
    );

    // Check if the conversion task was created successfully
    if (conversion_task_status != pdPASS) {
        // Handle error
    }

    // IO_task_handle = xTaskCreate(               IO_task,            // task function
    //                                             "IO_task",          // task name
    //                                             4096,               // stack depth
    //                                             NULL,               // task parameters
    //                                             CONVERSION_TASK_PRIORITY,       // task priority
    //                                             NULL);              // create the conversion task   




    // IO_queue = xQueueCreate(                    10,                 // queue length
    //                                             sizeof(uint16_t));    // queue item size


    // configure the GPIO
    gpio_config_t io_conf;                          // GPIO config struct
    io_conf.pin_bit_mask = (1ULL << BUTTON_GPIO);   // set the GPIO pin mask
    io_conf.mode = GPIO_MODE_INPUT;                 // set the GPIO mode to input
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;        // disable the pull up resistor  
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;   // disable the pull down resistor
    io_conf.intr_type = GPIO_INTR_DISABLE;          // disable the GPIO interrupt
    gpio_config(&io_conf);                          // configure the GPIO

    vTaskDelete(NULL); // delete the main task
}