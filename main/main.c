#include "main.h"
#include "adc.h"
#include "sigproc.h"

static const char *TAG = "MAIN"; 

gptimer_handle_t glob_tim_handle = NULL;      // global timer handle
TaskHandle_t adc_task_handle;         // global task handle for the ADC task
TaskHandle_t conversion_task_handle;  // global task handle for the conversion task
TaskHandle_t data_log_task_handle;          // global task handle for the IO task
QueueHandle_t adc_queue_handle;               // global queue handle for the ADC queue
QueueHandle_t data_log_queue_handle;                // global queue handle for the IO queue

bool record_state = false; // global variable to store the record state

static void button_isr_handler(void* arg)
{
    record_state = !record_state; // toggle the record state
    ESP_DRAM_LOGI(TAG, "Record state: %d", record_state); // log the record state
}

void app_main(void)
{   
    esp_err_t ret; // variable for error checking

    // configure the GPIO
    gpio_config_t io_conf;                          // GPIO config struct
    io_conf.pin_bit_mask = (1ULL << BUTTON_GPIO);   // set the GPIO pin mask
    io_conf.mode = GPIO_MODE_INPUT;                 // set the GPIO mode to input
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;       // disable the pull up resistor  
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;    // enable the pull down resistor
    io_conf.intr_type = GPIO_INTR_POSEDGE;          // set the interrupt type to rising edge
    gpio_config(&io_conf);                          // configure the GPIO

    io_conf.pin_bit_mask = (1ULL << LED_GPIO);      // set the GPIO pin mask
    io_conf.mode = GPIO_MODE_OUTPUT;                // set the GPIO mode to input
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;        // disable the pull up resistor  
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;   // disable the pull down resistor
    io_conf.intr_type = GPIO_INTR_POSEDGE;          // set the interrupt type to rising edge
    gpio_config(&io_conf);                          // configure the GPIO

    // configure the button interrupt
    ret = gpio_install_isr_service(0); // install the GPIO ISR service
    if(ret != ESP_OK) ESP_LOGE(TAG, "Failed to install the GPIO ISR service");
    ret = gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL); // add the ISR handler
    if(ret != ESP_OK) ESP_LOGE(TAG, "Failed to add the ISR handler");

    // Configure Timer
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_RESOLUTION, 
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &glob_tim_handle));
    ESP_ERROR_CHECK(gptimer_enable(glob_tim_handle));
    ESP_ERROR_CHECK(gptimer_start(glob_tim_handle));


    /// FreeRTOS initialization ///

    adc_queue_handle = xQueueCreate(            FRAMES_PER_CONVERSION,                  // queue length
                                                sizeof(adc_data_t));                    // queue item size
    if(adc_queue_handle == NULL) ESP_LOGE(TAG, "Failed to create ADC queue");
    else ESP_LOGI(TAG, "ADC queue created");

    data_log_queue_handle = xQueueCreate(       5 ,                                     // queue length
                                                sizeof(data_t));                        // queue item size
    if(data_log_queue_handle == NULL) ESP_LOGE(TAG, "Failed to create data log queue");
    else ESP_LOGI(TAG, "Data log queue created");

    // Create the ADC task
    BaseType_t adc_task_status = xTaskCreate(
        adc_read_task,                  // task function
        "adc_task",                     // task name
        8192,                           // stack depth
        NULL,                           // task parameters
        ADC_TASK_PRIORITY,              // task priority
        &adc_task_handle                // pointer to store the task handle
    );
    if (adc_task_status != pdPASS) ESP_LOGE(TAG, "Failed to create ADC task");
    else ESP_LOGI(TAG, "ADC task created");

    // Create the conversion task
    BaseType_t conversion_task_status = xTaskCreate(
        analog_to_freq_conversion_task, // task function
        "conversion_task",              // task name
        4096,                           // stack depth
        NULL,                           // task parameters
        CONVERSION_TASK_PRIORITY,       // task priority
        &conversion_task_handle         // pointer to store the task handle
    );
    if (conversion_task_status != pdPASS) ESP_LOGE(TAG, "Failed to create conversion task");
    else ESP_LOGI(TAG, "Conversion task created");

    BaseType_t data_log_task_handle = xTaskCreate(               
        data_log_task,                  // task function
        "IO_task",                      // task name
        4096,                           // stack depth
        NULL,                           // task parameters
        DATA_LOG_TASK_PRIORITY,         // task priority
        &data_log_task_handle           // pointer to store the task handle
    );        
    if (data_log_task_handle != pdPASS) ESP_LOGE(TAG, "Failed to create data log task");
    else ESP_LOGI(TAG, "Data log task created");

    vTaskDelete(NULL); // delete the main task
}